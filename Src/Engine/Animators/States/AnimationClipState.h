#pragma once

#include "Common.h"
#include "AnimationStateBase.h"

AWAY_NAMESPACE_BEGIN

class AnimationClipState : public AnimationStateBase
{
public:
	AnimationClipState(IAnimator* animator, AnimationClipNodeBase* animationClipNode);

	float getBlendWeight();
	unsigned int getCurrentFrame();
	unsigned int getNextFrame();

	void update(float time) override;
	void phase(float value) override;

protected:
	void updateTime(float time) override;
	virtual void updateFrames();

protected:
	float m_blendWeight;
	unsigned m_currentFrame;
	unsigned int m_nextFrame;
	unsigned int m_oldFrame;
	bool m_timeDir;
	bool m_framesDirty = true;

private:
	AnimationClipNodeBase* m_animationClipNode;
};

AWAY_NAMESPACE_END