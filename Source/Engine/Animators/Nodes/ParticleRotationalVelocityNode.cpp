#include "ParticleRotationalVelocityNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleRotationalVelocityState.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

const int ParticleRotationalVelocityNode::ROTATIONALVELOCITY_INDEX = 0;
const std::string ParticleRotationalVelocityNode::ROTATIONALVELOCITY_VECTOR3D = "RotationalVelocityVector3D";

ParticleRotationalVelocityNode::ParticleRotationalVelocityNode(ParticlePropertiesMode mode, Vector3D* rotationalVelocity) : ParticleNodeBase("ParticleRotationalVelocity", mode, 4)
{
	m_rotationalVelocity = rotationalVelocity;
}

void ParticleRotationalVelocityNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int rotationRegister = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, ROTATIONALVELOCITY_INDEX, REGISTER_INDEX(rotationRegister));

	unsigned int nrmVel = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(nrmVel, 1);
	unsigned int xAxis = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(xAxis, 1);
	unsigned int temp = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(temp, 1);
	unsigned int R_rev = regCache->getFreeVertexVectorTemp();
	regCache->removeVertexTempUsage(nrmVel);
	regCache->removeVertexTempUsage(xAxis);
	regCache->removeVertexTempUsage(temp);

	unsigned int cos = temp ^ Regs::w;
	unsigned int sin = R_rev ^ Regs::w;
	code.mov(nrmVel ^ Regs::xyz, rotationRegister);
	code.mov(nrmVel ^ Regs::w, regCache->m_vertexZeroConst);
	code.mul(cos, regCache->m_vertexTime, rotationRegister ^ Regs::w);
	code.sin(sin, cos);
	code.cos(cos, cos);
	code.mul(temp ^ Regs::xyz, sin, nrmVel);
	code.mul(R_rev ^ Regs::xyz, sin, nrmVel);
	code.neg(R_rev ^ Regs::xyz, R_rev);

	// nrmVel and xAxis are used as temp register
	code.crs(nrmVel ^ Regs::xyz, temp, regCache->m_scaleAndRotateTarget);
	code.mul(xAxis ^ Regs::xyz, cos, regCache->m_scaleAndRotateTarget);
	code.add(nrmVel ^ Regs::xyz, nrmVel, xAxis);
	code.dp3(xAxis ^ Regs::w, temp, regCache->m_scaleAndRotateTarget);
	code.neg(nrmVel ^ Regs::w, xAxis ^ Regs::w);
	code.crs(temp ^ Regs::xyz, nrmVel, R_rev);

	// use cos as R_rev.w
	code.mul(xAxis, nrmVel, cos);
	code.add(temp ^ Regs::xyz, temp, xAxis);
	code.mul(xAxis ^ Regs::xyz, nrmVel ^ Regs::w, R_rev);
	code.add(regCache->m_scaleAndRotateTarget ^ Regs::xyz, temp, xAxis);

	for (auto rotationReg : regCache->m_rotationRegisters)
	{
		code.mov(nrmVel ^ Regs::xyz, rotationRegister);
		code.mov(nrmVel ^ Regs::w, regCache->m_vertexZeroConst);
		code.mul(cos, regCache->m_vertexTime, rotationRegister ^ Regs::w);
		code.sin(sin, cos);
		code.cos(cos, cos);
		code.mul(temp ^ Regs::xyz, sin, nrmVel);
		code.mul(R_rev ^ Regs::xyz, sin, nrmVel);
		code.neg(R_rev ^ Regs::xyz, R_rev);
		code.crs(nrmVel ^ Regs::xyz, temp, rotationReg);
		code.mul(xAxis ^ Regs::xyz, cos, rotationReg);
		code.add(nrmVel ^ Regs::xyz, nrmVel, xAxis);
		code.dp3(xAxis ^ Regs::w, temp, rotationReg);
		code.neg(nrmVel ^ Regs::w, xAxis ^ Regs::w);
		code.crs(temp ^ Regs::xyz, nrmVel, R_rev);
		code.mul(xAxis, nrmVel, cos);
		code.add(temp ^ Regs::xyz, temp, xAxis);
		code.mul(xAxis ^ Regs::xyz, nrmVel ^ Regs::w, R_rev);
		code.add(rotationReg ^ Regs::xyz, temp, xAxis);
	}
}

AnimationStateBase* ParticleRotationalVelocityNode::createAnimationState(IAnimator* animator)
{
	return new ParticleRotationalVelocityState(animator, this);
}

void ParticleRotationalVelocityNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	// (x,y,z) is rotation axis, w is cycle duration
	Vector3D& rotate = param[ROTATIONALVELOCITY_VECTOR3D];
	if (rotate.getLength() <= 0)
		rotate.m_z = 1;
	else
		rotate.normalize();

	m_oneData[0] = rotate.m_x;
	m_oneData[1] = rotate.m_y;
	m_oneData[2] = rotate.m_z;
	m_oneData[3] = MathConsts::PI / rotate.m_w;
}