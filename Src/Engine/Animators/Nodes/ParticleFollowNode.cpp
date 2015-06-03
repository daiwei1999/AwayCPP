#include "ParticleFollowNode.h"
#include "ParticleAnimationSet.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleFollowState.h"

USING_AWAY_NAMESPACE

const int ParticleFollowNode::FOLLOW_POSITION_INDEX = 0;
const int ParticleFollowNode::FOLLOW_ROTATION_INDEX = 1;

ParticleFollowNode::ParticleFollowNode(bool usesPosition, bool usesRotation, bool smooth) : ParticleNodeBase("ParticleFollow", ParticlePropertiesMode::LOCAL_DYNAMIC, (usesPosition && usesRotation) ? 6 : 3, ParticleAnimationSet::POST_PRIORITY)
{
	m_usesPosition = usesPosition;
	m_usesRotation = usesRotation;
	m_smooth = smooth;
}

void ParticleFollowNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	// TODO: use Quaternion to implement this function
	if (m_usesRotation)
	{
		unsigned int rotationAttribute = regCache->getFreeVertexAttribute();
		regCache->setRegisterIndex(this, FOLLOW_ROTATION_INDEX, REGISTER_INDEX(rotationAttribute));

		unsigned int temp1 = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp1, 1);
		unsigned int temp2 = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp2, 1);
		unsigned int temp3 = regCache->getFreeVertexVectorTemp();

		// x axis
		code.mov(temp1, regCache->m_vertexZeroConst);
		code.mov(temp1 ^ Regs::x, regCache->m_vertexOneConst);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.sin(temp3 ^ Regs::y, rotationAttribute ^ Regs::x);
		code.cos(temp3 ^ Regs::z, rotationAttribute ^ Regs::x);
		code.mov(temp2 ^ Regs::x, regCache->m_vertexZeroConst);
		code.mov(temp2 ^ Regs::y, temp3 ^ Regs::z);
		code.neg(temp2 ^ Regs::z, temp3 ^ Regs::y);

		unsigned int temp4 = 0;
		if (regCache->m_hasBillboard)
		{
			regCache->addVertexTempUsages(temp3, 1);
			temp4 = regCache->getFreeVertexVectorTemp();
			regCache->removeVertexTempUsage(temp3);
			code.m33(temp4 ^ Regs::xyz, regCache->m_positionTarget, temp1);
		}
		else
		{
			code.m33(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp1);
			for (auto rotationReg : regCache->m_rotationRegisters)
				code.m33(rotationReg ^ Regs::xyz, rotationReg, temp1);
		}

		// y axis
		code.mov(temp1, regCache->m_vertexZeroConst);
		code.cos(temp1 ^ Regs::x, rotationAttribute ^ Regs::y);
		code.sin(temp1 ^ Regs::z, rotationAttribute ^ Regs::y);
		code.mov(temp2, regCache->m_vertexZeroConst);
		code.mov(temp2 ^ Regs::y, regCache->m_vertexOneConst);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.neg(temp3 ^ Regs::x, temp1 ^ Regs::z);
		code.mov(temp3 ^ Regs::z, temp1 ^ Regs::x);

		if (regCache->m_hasBillboard)
			code.m33(temp4 ^ Regs::xyz, temp4, temp1);
		else
		{
			code.m33(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp1);
			for (auto rotationReg : regCache->m_rotationRegisters)
				code.m33(rotationReg ^ Regs::xyz, rotationReg, temp1);
		}

		// z axis
		code.mov(temp2, regCache->m_vertexZeroConst);
		code.sin(temp2 ^ Regs::x, rotationAttribute ^ Regs::z);
		code.cos(temp2 ^ Regs::y, rotationAttribute ^ Regs::z);
		code.mov(temp1, regCache->m_vertexZeroConst);
		code.mov(temp1 ^ Regs::x, temp2 ^ Regs::y);
		code.neg(temp1 ^ Regs::y, temp2 ^ Regs::x);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.mov(temp3 ^ Regs::z, regCache->m_vertexOneConst);

		if (regCache->m_hasBillboard)
		{
			code.m33(temp4 ^ Regs::xyz, temp4, temp1);
			code.sub(temp4 ^ Regs::xyz, temp4, regCache->m_positionTarget);
			code.add(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp4);
		}
		else
		{
			code.m33(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp1);
			for (auto rotationReg : regCache->m_rotationRegisters)
				code.m33(rotationReg ^ Regs::xyz, rotationReg, temp1);
		}

		regCache->removeVertexTempUsage(temp2);
		regCache->removeVertexTempUsage(temp1);
	}

	if (m_usesPosition)
	{
		unsigned int positionAttribute = regCache->getFreeVertexAttribute();
		regCache->setRegisterIndex(this, FOLLOW_POSITION_INDEX, REGISTER_INDEX(positionAttribute));

		code.add(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, positionAttribute);
	}
}

AnimationStateBase* ParticleFollowNode::createAnimationState(IAnimator* animator)
{
	return new ParticleFollowState(animator, this);
}