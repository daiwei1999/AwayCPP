#pragma once

#include "Common.h"
#include "AnimationClipState.h"
#include "SkeletonPose.h"

AWAY_NAMESPACE_BEGIN

class SkeletonClipState : public AnimationClipState
{
public:
	SkeletonClipState(IAnimator* animator, SkeletonClipNode* skeletonClipNode);

	SkeletonPose* getCurrentPose();
	SkeletonPose* getNextPose();
	SkeletonPose& getSkeletonPose(Skeleton* skeleton);

protected:
	void updateTime(float time) override;
	void updateFrames() override;

private:
	void updateSkeletonPose(Skeleton* skeleton);

private:
	SkeletonPose m_skeletonPose;
	bool m_skeletonPoseDirty = true;
	SkeletonPose* m_currentPose;
	SkeletonPose* m_nextPose;
};

AWAY_NAMESPACE_END