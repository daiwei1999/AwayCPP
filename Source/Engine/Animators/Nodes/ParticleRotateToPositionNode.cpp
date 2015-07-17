#include "ParticleRotateToPositionNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleRotateToPositionState.h"

USING_AWAY_NAMESPACE

const int ParticleRotateToPositionNode::MATRIX_INDEX = 0;
const int ParticleRotateToPositionNode::POSITION_INDEX = 1;
const std::string ParticleRotateToPositionNode::POSITION_VECTOR3D = "RotateToPositionVector3D";

ParticleRotateToPositionNode::ParticleRotateToPositionNode(ParticlePropertiesMode mode, Vector3D* position) : ParticleNodeBase("ParticleRotateToPosition", mode, 3, 3)
{
	m_position = position;
}

void ParticleRotateToPositionNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int positionAttribute = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, POSITION_INDEX, REGISTER_INDEX(positionAttribute));

	if (regCache->m_hasBillboard)
	{
		unsigned int temp1 = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp1, 1);
		unsigned int temp2 = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp2, 1);
		unsigned int temp3 = regCache->getFreeVertexVectorTemp();
		regCache->removeVertexTempUsage(temp1);
		regCache->removeVertexTempUsage(temp2);

		unsigned int rotationMatrixReg = regCache->getFreeVertexConstant();
		regCache->getFreeVertexConstant();
		regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, MATRIX_INDEX, REGISTER_INDEX(rotationMatrixReg));

		// process the position
		code.sub(temp1 ^ Regs::xyz, positionAttribute, regCache->m_positionTarget);
		code.m33(temp1 ^ Regs::xyz, temp1, rotationMatrixReg);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.mov(temp3 ^ Regs::xy, temp1);
		code.nrm(temp3 ^ Regs::xyz, temp3);

		// only process z axis
		code.mov(temp2, regCache->m_vertexZeroConst);
		code.mov(temp2 ^ Regs::x, temp3 ^ Regs::y);
		code.mov(temp2 ^ Regs::y, temp3 ^ Regs::x);
		code.mov(temp1, regCache->m_vertexZeroConst);
		code.mov(temp1 ^ Regs::x, temp3 ^ Regs::x);
		code.neg(temp1 ^ Regs::y, temp3 ^ Regs::y);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.mov(temp3 ^ Regs::z, regCache->m_vertexOneConst);
		code.m33(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp1);
		for (auto rotationReg : regCache->m_rotationRegisters)
			code.m33(rotationReg ^ Regs::xyz, rotationReg, temp1);
	}
	else
	{
		unsigned int nrmDirection = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(nrmDirection, 1);
		unsigned int temp = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp, 1);
		unsigned int R = regCache->getFreeVertexVectorTemp();
		regCache->removeVertexTempUsage(nrmDirection);
		regCache->removeVertexTempUsage(temp);

		unsigned int cos = temp ^ Regs::x;
		unsigned int sin = temp ^ Regs::y;
		unsigned int o_temp = temp ^ Regs::z;
		unsigned int tempSingle = temp ^ Regs::w;
		code.sub(nrmDirection ^ Regs::xyz, positionAttribute, regCache->m_positionTarget);
		code.nrm(nrmDirection ^ Regs::xyz, nrmDirection);
		code.mov(sin, nrmDirection ^ Regs::y);
		code.mul(cos, sin, sin);
		code.sub(cos, regCache->m_vertexOneConst, cos);
		code.sqt(cos, cos);

		code.mul(R ^ Regs::x, cos, regCache->m_scaleAndRotateTarget ^ Regs::y);
		code.mul(R ^ Regs::y, sin, regCache->m_scaleAndRotateTarget ^ Regs::z);
		code.mul(R ^ Regs::z, sin, regCache->m_scaleAndRotateTarget ^ Regs::y);
		code.mul(R ^ Regs::w, cos, regCache->m_scaleAndRotateTarget ^ Regs::z);

		code.sub(regCache->m_scaleAndRotateTarget ^ Regs::y, R ^ Regs::x, R ^ Regs::y);
		code.add(regCache->m_scaleAndRotateTarget ^ Regs::z, R ^ Regs::z, R ^ Regs::w);

		code.abs(R ^ Regs::y, nrmDirection ^ Regs::y);
		code.sge(R ^ Regs::z, R ^ Regs::y, regCache->m_vertexOneConst);
		code.mul(R ^ Regs::x, R ^ Regs::y, nrmDirection ^ Regs::y);

		// judge if nrmDirection=(0,1,0)
		code.mov(nrmDirection ^ Regs::y, regCache->m_vertexZeroConst);
		code.dp3(sin, nrmDirection, nrmDirection);
		code.sge(tempSingle, regCache->m_vertexZeroConst, sin);

		code.mov(nrmDirection ^ Regs::y, regCache->m_vertexZeroConst);
		code.nrm(nrmDirection ^ Regs::xyz, nrmDirection);
		code.sub(sin, regCache->m_vertexOneConst, tempSingle);
		code.mul(sin, sin, nrmDirection ^ Regs::x);

		code.mov(cos, nrmDirection ^ Regs::z);
		code.neg(cos, cos);
		code.sub(o_temp, regCache->m_vertexOneConst, cos);
		code.mul(o_temp, R ^ Regs::x, tempSingle);
		code.add(cos, cos, o_temp);

		code.mul(R ^ Regs::x, cos, regCache->m_scaleAndRotateTarget ^ Regs::x);
		code.mul(R ^ Regs::y, sin, regCache->m_scaleAndRotateTarget ^ Regs::z);
		code.mul(R ^ Regs::z, sin, regCache->m_scaleAndRotateTarget ^ Regs::x);
		code.mul(R ^ Regs::w, cos, regCache->m_scaleAndRotateTarget ^ Regs::z);

		code.sub(regCache->m_scaleAndRotateTarget ^ Regs::x, R ^ Regs::x, R ^ Regs::y);
		code.add(regCache->m_scaleAndRotateTarget ^ Regs::z, R ^ Regs::z, R ^ Regs::w);

		for (auto rotationReg : regCache->m_rotationRegisters)
		{
			// just repeat the calculate above, because of the limited registers, no need to optimize
			code.sub(nrmDirection ^ Regs::xyz, positionAttribute, regCache->m_positionTarget);
			code.nrm(nrmDirection ^ Regs::xyz, nrmDirection);
			code.mov(sin, nrmDirection ^ Regs::y);
			code.mul(cos, sin, sin);
			code.sub(cos, regCache->m_vertexOneConst, cos);
			code.sqt(cos, cos);
			code.mul(R ^ Regs::x, cos, rotationReg ^ Regs::y);
			code.mul(R ^ Regs::y, sin, rotationReg ^ Regs::z);
			code.mul(R ^ Regs::z, sin, rotationReg ^ Regs::y);
			code.mul(R ^ Regs::w, cos, rotationReg ^ Regs::z);
			code.sub(rotationReg ^ Regs::y, R ^ Regs::x, R ^ Regs::y);
			code.add(rotationReg ^ Regs::z, R ^ Regs::z, R ^ Regs::w);
			code.abs(R ^ Regs::y, nrmDirection ^ Regs::y);
			code.sge(R ^ Regs::z, R ^ Regs::y, regCache->m_vertexOneConst);
			code.mul(R ^ Regs::x, R ^ Regs::y, nrmDirection ^ Regs::y);
			code.mov(nrmDirection ^ Regs::y, regCache->m_vertexZeroConst);
			code.dp3(sin, nrmDirection, nrmDirection);
			code.sge(tempSingle, regCache->m_vertexZeroConst, sin);
			code.mov(nrmDirection ^ Regs::y, regCache->m_vertexZeroConst);
			code.nrm(nrmDirection ^ Regs::xyz, nrmDirection);
			code.sub(sin, regCache->m_vertexOneConst, tempSingle);
			code.mul(sin, sin, nrmDirection ^ Regs::x);
			code.mov(cos, nrmDirection ^ Regs::z);
			code.neg(cos, cos);
			code.sub(o_temp, regCache->m_vertexOneConst, cos);
			code.mul(o_temp, R ^ Regs::x, tempSingle);
			code.add(cos, cos, o_temp);
			code.mul(R ^ Regs::x, cos, rotationReg ^ Regs::x);
			code.mul(R ^ Regs::y, sin, rotationReg ^ Regs::z);
			code.mul(R ^ Regs::z, sin, rotationReg ^ Regs::x);
			code.mul(R ^ Regs::w, cos, rotationReg ^ Regs::z);
			code.sub(rotationReg ^ Regs::x, R ^ Regs::x, R ^ Regs::y);
			code.add(rotationReg ^ Regs::z, R ^ Regs::z, R ^ Regs::w);
		}
	}
}

AnimationStateBase* ParticleRotateToPositionNode::createAnimationState(IAnimator* animator)
{
	return new ParticleRotateToPositionState(animator, this);
}

void ParticleRotateToPositionNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& offset = param[POSITION_VECTOR3D];
	m_oneData[0] = offset.m_x;
	m_oneData[1] = offset.m_y;
	m_oneData[2] = offset.m_z;
}