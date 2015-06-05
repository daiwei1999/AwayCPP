#include "ParticleAccelerationNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleAccelerationState.h"

USING_AWAY_NAMESPACE

const int ParticleAccelerationNode::ACCELERATION_INDEX = 0;
const std::string ParticleAccelerationNode::ACCELERATION_VECTOR3D = "AccelerationVector3D";

ParticleAccelerationNode::ParticleAccelerationNode(ParticlePropertiesMode mode, Vector3D* acceleration) : ParticleNodeBase("ParticleAcceleration", mode, 3)
{
	m_acceleration = acceleration;
}

void ParticleAccelerationNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int accelerationReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, ACCELERATION_INDEX, REGISTER_INDEX(accelerationReg));

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	code.mul(temp, regCache->m_vertexTime, accelerationReg);

	if (regCache->m_needVelocity)
	{
		regCache->addVertexTempUsages(temp, 1);
		unsigned int temp2 = regCache->getFreeVertexVectorTemp();
		code.mul(temp2, temp, regCache->m_vertexTwoConst);
		code.add(regCache->m_velocityTarget ^ Regs::xyz, regCache->m_velocityTarget, temp2);
		regCache->removeVertexTempUsage(temp);
	}

	code.mul(temp, temp, regCache->m_vertexTime);
	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget, temp);
}

AnimationStateBase* ParticleAccelerationNode::createAnimationState(IAnimator* animator)
{
	return new ParticleAccelerationState(animator, this);
}

void ParticleAccelerationNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& acceleration = param[ACCELERATION_VECTOR3D];
	m_oneData[0] = acceleration.m_x / 2;
	m_oneData[1] = acceleration.m_y / 2;
	m_oneData[2] = acceleration.m_z / 2;
}