#include "ParticlePositionState.h"
#include "ParticlePositionNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticlePositionState::ParticlePositionState(IAnimator* animator, ParticlePositionNode* particlePositionNode) : ParticleStateBase(animator, particlePositionNode)
{

}

void ParticlePositionState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticlePositionNode* particlePositionNode = static_cast<ParticlePositionNode*>(m_animationNode);
	if (particlePositionNode->getMode() == ParticlePropertiesMode::LOCAL_DYNAMIC && !m_dynamicPropertiesDirty[subGeometry])
		updateDynamicProperties(subGeometry);

	int index = regCache->getRegisterIndex(m_animationNode, ParticlePositionNode::POSITION_INDEX);
	if (particlePositionNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		Vector3D* position = particlePositionNode->m_position;
		regCache->setVertexConst(index, position->m_x, position->m_y, position->m_z);
	}
	else
		subGeometry->activateVertexBuffer(index, particlePositionNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
}