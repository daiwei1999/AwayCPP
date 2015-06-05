#include "ParticlePositionNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticlePositionState.h"

USING_AWAY_NAMESPACE

const int ParticlePositionNode::POSITION_INDEX = 0;
const std::string ParticlePositionNode::POSITION_VECTOR3D = "PositionVector3D";

ParticlePositionNode::ParticlePositionNode(ParticlePropertiesMode mode, Vector3D* position) : ParticleNodeBase("ParticlePosition", mode, 3)
{
	m_position = position;
}

void ParticlePositionNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int positionReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, POSITION_INDEX, REGISTER_INDEX(positionReg));

	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget, positionReg);
}

AnimationStateBase* ParticlePositionNode::createAnimationState(IAnimator* animator)
{
	return new ParticlePositionState(animator, this);
}

void ParticlePositionNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& offset = param[POSITION_VECTOR3D];
	m_oneData[0] = offset.m_x;
	m_oneData[1] = offset.m_y;
	m_oneData[2] = offset.m_z;
}