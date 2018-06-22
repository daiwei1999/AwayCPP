#include "ParticleSpriteSheetNode.h"
#include "ParticleAnimationSet.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleSpriteSheetState.h"

USING_AWAY_NAMESPACE

const int ParticleSpriteSheetNode::UV_INDEX_0 = 0;
const int ParticleSpriteSheetNode::UV_INDEX_1 = 1;
const std::string ParticleSpriteSheetNode::UV_VECTOR3D = "UVVector3D";

ParticleSpriteSheetNode::ParticleSpriteSheetNode(ParticlePropertiesMode mode, bool usesCycle, bool usesPhase, int numColumns, int numRows, float cycleDuration, float cyclePhase) : 
												 ParticleNodeBase("ParticleSpriteSheet", mode, usesCycle ? (usesPhase ? 3 : 2) : 0, ParticleAnimationSet::POST_PRIORITY + 1)
{
	m_usesCycle = usesCycle;
	m_usesPhase = usesPhase;
	m_numColumns = numColumns;
	m_numRows = numRows;
	m_cycleDuration = cycleDuration;
	m_cyclePhase = cyclePhase;
}

void ParticleSpriteSheetNode::getAGALUVCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int uvParamConst1 = regCache->getFreeVertexConstant();
	unsigned int uvParamConst2 = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, UV_INDEX_0, REGISTER_INDEX(uvParamConst1));
	regCache->setRegisterIndex(this, UV_INDEX_1, REGISTER_INDEX(uvParamConst2));

	unsigned int uTotal = uvParamConst1 ^ Regs::x;
	unsigned int uStep = uvParamConst1 ^ Regs::y;
	unsigned int vStep = uvParamConst1 ^ Regs::z;
	unsigned int uSpeed = uvParamConst2 ^ Regs::x;
	unsigned int cycle = uvParamConst2 ^ Regs::y;

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	unsigned int time = temp ^ Regs::x;
	unsigned int vOffset = temp ^ Regs::y;
	unsigned int temp1 = temp ^ Regs::z;
	unsigned int temp2 = temp ^ Regs::w;
	unsigned int u = regCache->m_uvTarget ^ Regs::x;
	unsigned int v = regCache->m_uvTarget ^ Regs::y;

	code.mul(u, u, uStep);

	if (m_numRows > 1)
		code.mul(v, v, vStep);

	if (m_usesCycle)
	{
		if (m_usesPhase)
			code.add(time, regCache->m_vertexTime, uvParamConst2 ^ Regs::z);
		else
			code.mov(time, regCache->m_vertexTime);

		code.div(time, time, cycle);
		code.frc(time, time);
		code.mul(time, time, cycle);
		code.mul(temp1, time, uSpeed);
	}
	else
		code.mul(temp1, regCache->m_vertexLife, uTotal);

	if (m_numRows > 1)
	{
		code.frc(temp2, temp1);
		code.sub(vOffset, temp1, temp2);
		code.mul(vOffset, vOffset, vStep);
		code.add(v, v, vOffset);
	}

	code.div(temp2, temp1, uStep);
	code.frc(temp1, temp2);
	code.sub(temp2, temp2, temp1);
	code.mul(temp1, temp2, uStep);

	if (m_numRows > 1)
		code.frc(temp1, temp1);

	code.add(u, u, temp1);
}

AnimationStateBase* ParticleSpriteSheetNode::createAnimationState(IAnimator* animator)
{
	return new ParticleSpriteSheetState(animator, this);
}

void ParticleSpriteSheetNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	particleAnimationSet->m_hasUVNode = true;
}

void ParticleSpriteSheetNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	if (m_usesCycle)
	{
		Vector3D& uvCycle = param[UV_VECTOR3D];
		m_oneData[0] = m_numRows / uvCycle.m_x;
		m_oneData[1] = uvCycle.m_x;
		if (m_usesPhase)
			m_oneData[2] = uvCycle.m_y;
	}
}