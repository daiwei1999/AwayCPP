#include "SkeletonClipNode.h"
#include "SkeletonClipState.h"
#include "SkeletonPose.h"

USING_AWAY_NAMESPACE

SkeletonClipNode::SkeletonClipNode()
{

}

void SkeletonClipNode::addFrame(SkeletonPose* skeletonPose, float duration)
{
	m_frames.push_back(skeletonPose);
	m_durations.push_back(duration);

	m_numFrames = m_durations.size();

	m_stitchDirty = true;
}

AnimationStateBase* SkeletonClipNode::createAnimationState(IAnimator* animator)
{
	return new SkeletonClipState(animator, this);
}

void SkeletonClipNode::updateStitch()
{
	AnimationClipNodeBase::updateStitch();

	unsigned int i = m_numFrames - 1;
	while (i--)
	{
		m_totalDuration += m_durations[i];
		Vector3D& p1 = m_frames[i]->m_jointPoses[0].m_translation;
		Vector3D& p2 = m_frames[i + 1]->m_jointPoses[0].m_translation;
		m_totalDelta.m_x += (p2.m_x - p1.m_x);
		m_totalDelta.m_y += (p2.m_y - p1.m_y);
		m_totalDelta.m_z += (p2.m_z - p1.m_z);
	}

	if (m_stitchFinalFrame && m_looping)
	{
		m_totalDuration += m_durations[m_numFrames - 1];
		if (m_numFrames > 1)
		{
			Vector3D& p1 = m_frames[0]->m_jointPoses[0].m_translation;
			Vector3D& p2 = m_frames[1]->m_jointPoses[0].m_translation;
			m_totalDelta.m_x += (p2.m_x - p1.m_x);
			m_totalDelta.m_y += (p2.m_y - p1.m_y);
			m_totalDelta.m_z += (p2.m_z - p1.m_z);
		}
	}
}