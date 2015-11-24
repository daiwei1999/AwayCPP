#pragma once

#include "Common.h"
#include "AnimationClipNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A skeleton animation node containing time-based animation data as individual skeleton poses.
 */
class SkeletonClipNode : public AnimationClipNodeBase
{
public:
	SkeletonClipNode();

	std::vector<SkeletonPose*>& getFrames() { return m_frames; }
	void addFrame(SkeletonPose* skeletonPose, float duration);
	AnimationStateBase* createAnimationState(IAnimator* animator) override;

protected:
	void updateStitch() override;

public:
	bool m_highQuality = false;

private:
	std::vector<SkeletonPose*> m_frames;
};

AWAY_NAMESPACE_END