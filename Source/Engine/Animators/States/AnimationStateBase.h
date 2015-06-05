#pragma once

#include "Common.h"
#include "IAnimationState.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

class AnimationStateBase : public IAnimationState
{
public:
	AnimationStateBase(IAnimator* animator, AnimationNodeBase* animationNode);

	Vector3D& getPositionDelta() override;
	void offset(float startTime) override;
	void update(float time) override;
	void phase(float value) override;

protected:
	virtual void updateTime(float time);
	virtual void updatePositionDelta();

protected:
	IAnimator* m_animator;
	AnimationNodeBase* m_animationNode;
	Vector3D m_rootDelta;
	bool m_positionDeltaDirty = true;
	float m_time = 0;
	float m_startTime = 0;
};

AWAY_NAMESPACE_END