#include "AnimationStateBase.h"

USING_AWAY_NAMESPACE

AnimationStateBase::AnimationStateBase(IAnimator* animator, AnimationNodeBase* animationNode)
{
	m_animator = animator;
	m_animationNode = animationNode;
}

void AnimationStateBase::offset(float startTime)
{
	m_startTime = startTime;
}

void AnimationStateBase::update(float time)
{
	if (m_time != time - m_startTime)
		updateTime(time);
}

void AnimationStateBase::phase(float value)
{

}

void AnimationStateBase::updateTime(float time)
{
	m_time = time - m_startTime;
}