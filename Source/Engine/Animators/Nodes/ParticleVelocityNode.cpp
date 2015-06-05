#include "ParticleVelocityNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleVelocityState.h"

USING_AWAY_NAMESPACE

const int ParticleVelocityNode::VELOCITY_INDEX = 0;
const std::string ParticleVelocityNode::VELOCITY_VECTOR3D = "VelocityVector3D";

ParticleVelocityNode::ParticleVelocityNode(ParticlePropertiesMode mode, Vector3D* velocity) : ParticleNodeBase("ParticleVelocity", mode, 3)
{
	m_velocity = velocity;
}

void ParticleVelocityNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int velocityReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, VELOCITY_INDEX, REGISTER_INDEX(velocityReg));

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	code.mul(temp, regCache->m_vertexTime, velocityReg);
	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget, temp);

	if (regCache->m_needVelocity)
		code.add(regCache->m_velocityTarget ^ Regs::xyz, regCache->m_velocityTarget, velocityReg);
}

AnimationStateBase* ParticleVelocityNode::createAnimationState(IAnimator* animator)
{
	return new ParticleVelocityState(animator, this);
}

void ParticleVelocityNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& velocity = param[VELOCITY_VECTOR3D];
	m_oneData[0] = velocity.m_x;
	m_oneData[1] = velocity.m_y;
	m_oneData[2] = velocity.m_z;
}