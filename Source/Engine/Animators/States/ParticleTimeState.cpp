#include "ParticleTimeState.h"
#include "ParticleTimeNode.h"
#include "AnimationSubGeometry.h"
#include "AnimationRegisterCache.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleTimeState::ParticleTimeState(IAnimator* animator, ParticleTimeNode* particleTimeNode) : ParticleStateBase(animator, particleTimeNode, true)
{

}

void ParticleTimeState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleTimeNode::TIME_STREAM_INDEX), static_cast<ParticleTimeNode*>(m_animationNode)->m_dataOffset, context, VertexBufferFormat::FLOAT_4);
	regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleTimeNode::TIME_CONSTANT_INDEX), m_time * 0.001f);
}