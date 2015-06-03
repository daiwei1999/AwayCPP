#include "ParticleAccelerationState.h"
#include "ParticleAccelerationNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleAccelerationState::ParticleAccelerationState(IAnimator* animator, ParticleAccelerationNode* particleAccelerationNode) : ParticleStateBase(animator, particleAccelerationNode)
{

}

void ParticleAccelerationState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	int index = regCache->getRegisterIndex(m_animationNode, ParticleAccelerationNode::ACCELERATION_INDEX);

	ParticleAccelerationNode* particleAccelerationNode = static_cast<ParticleAccelerationNode*>(m_animationNode);
	if (particleAccelerationNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
		subGeometry->activateVertexBuffer(index, particleAccelerationNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
	else
	{
		Vector3D* acceleration = particleAccelerationNode->m_acceleration;
		regCache->setVertexConst(index, acceleration->m_x * 0.5f, acceleration->m_y * 0.5f, acceleration->m_z * 0.5f);
	}
}