#include "AnimationStateBase.h"

USING_AWAY_NAMESPACE

AnimationStateBase::AnimationStateBase(IAnimator* animator, AnimationNodeBase* animationNode)
{
	m_animator = animator;
	m_animationNode = animationNode;
}

Vector3D& AnimationStateBase::getPositionDelta()
{
	if (m_positionDeltaDirty)
		updatePositionDelta();

	return m_rootDelta;
}

void AnimationStateBase::offset(float startTime)
{
	m_startTime = startTime;
	m_positionDeltaDirty = true;
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
	m_positionDeltaDirty = true;
}

void AnimationStateBase::updatePositionDelta()
{

}