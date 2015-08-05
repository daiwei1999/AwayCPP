#include "ParticleVelocityState.h"
#include "ParticleVelocityNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleVelocityState::ParticleVelocityState(IAnimator* animator, ParticleVelocityNode* particleVelocityNode) : ParticleStateBase(animator, particleVelocityNode)
{

}

void ParticleVelocityState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticleVelocityNode* particleVelocityNode = static_cast<ParticleVelocityNode*>(m_animationNode);
	int index = regCache->getRegisterIndex(m_animationNode, ParticleVelocityNode::VELOCITY_INDEX);
	if (particleVelocityNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		Vector3D* velocity = particleVelocityNode->m_velocity;
		regCache->setVertexConst(index, velocity->m_x, velocity->m_y, velocity->m_z);
	}
	else
		subGeometry->activateVertexBuffer(index, particleVelocityNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
}