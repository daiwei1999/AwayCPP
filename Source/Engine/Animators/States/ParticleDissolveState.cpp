#include "ParticleDissolveState.h"
#include "ParticleDissolveNode.h"
#include "AnimationRegisterCache.h"

USING_AWAY_NAMESPACE

ParticleDissolveState::ParticleDissolveState(IAnimator* animator, ParticleDissolveNode* particleDissolveNode) : ParticleStateBase(animator, particleDissolveNode, true)
{

}

void ParticleDissolveState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticleDissolveNode* particleDissolveNode = static_cast<ParticleDissolveNode*>(m_animationNode);
	float dt = (m_time - particleDissolveNode->m_timeOffset) * particleDissolveNode->m_speed;
	regCache->setFragmentConst(regCache->getRegisterIndex(m_animationNode, ParticleDissolveNode::DISSOLVE_INDEX), 0.299f, 0.587f, 0.114f, 1 - dt);
}