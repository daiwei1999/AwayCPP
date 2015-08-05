#include "ParticleStateBase.h"
#include "ParticleAnimator.h"
#include "ParticleNodeBase.h"
#include "AnimationSubGeometry.h"
#include "ParticleAnimationData.h"

USING_AWAY_NAMESPACE

ParticleStateBase::ParticleStateBase(IAnimator* animator, ParticleNodeBase* particleNode, bool needUpdateTime) : AnimationStateBase(animator, particleNode)
{
	m_needUpdateTime = needUpdateTime;
}