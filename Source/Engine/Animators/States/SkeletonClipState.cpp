#include "SkeletonClipState.h"
#include "SkeletonClipNode.h"
#include "Skeleton.h"

USING_AWAY_NAMESPACE

SkeletonClipState::SkeletonClipState(IAnimator* animator, SkeletonClipNode* skeletonClipNode) : AnimationClipState(animator, skeletonClipNode)
{

}

SkeletonPose* SkeletonClipState::getCurrentPose()
{
	if (m_framesDirty)
		updateFrames();

	return m_currentPose;
}

SkeletonPose* SkeletonClipState::getNextPose()
{
	if (m_framesDirty)
		updateFrames();

	return m_nextPose;
}

SkeletonPose& SkeletonClipState::getSkeletonPose(Skeleton* skeleton)
{
	if (m_skeletonPoseDirty)
		updateSkeletonPose(skeleton);

	return m_skeletonPose;
}

void SkeletonClipState::updateTime(float time)
{
	m_skeletonPoseDirty = true;
	AnimationClipState::updateTime(time);
}

void SkeletonClipState::updateFrames()
{
	AnimationClipState::updateFrames();

	SkeletonClipNode* skeletonClipNode = static_cast<SkeletonClipNode*>(m_animationNode);
	std::vector<SkeletonPose*>& frames = skeletonClipNode->getFrames();
	m_currentPose = frames[m_currentFrame];

	if (skeletonClipNode->getLooping() && m_nextFrame >= skeletonClipNode->getLastFrame())
		m_nextPose = frames[0];
	else
		m_nextPose = frames[m_nextFrame];
}

void SkeletonClipState::updateSkeletonPose(Skeleton* skeleton)
{
	m_skeletonPoseDirty = false;

	SkeletonClipNode* skeletonClipNode = static_cast<SkeletonClipNode*>(m_animationNode);
	if (skeletonClipNode->getTotalDuration() == 0)
		return;

	if (m_framesDirty)
		updateFrames();

	std::vector<JointPose>& currentPose = m_currentPose->m_jointPoses;
	std::vector<JointPose>& nextPose = m_nextPose->m_jointPoses;
	std::vector<JointPose>& endPoses = m_skeletonPose.m_jointPoses;
	int numJoints = skeleton->m_joints.size();
	endPoses.resize(numJoints);

	for (int i = 0; i < numJoints; i++)
	{
		JointPose& pose1 = currentPose[i];
		JointPose& pose2 = nextPose[i];
		JointPose& endPose = endPoses[i];

		// interpolation of orientation
		if (skeletonClipNode->m_highQuality)
			endPose.m_orientation.slerp(pose1.m_orientation, pose2.m_orientation, m_blendWeight);
		else
			endPose.m_orientation.lerp(pose1.m_orientation, pose2.m_orientation, m_blendWeight);

		// interpolation of translation
		Vector3D& tr = endPose.m_translation;
		Vector3D& p1 = pose1.m_translation;
		Vector3D& p2 = pose2.m_translation;
		tr.m_x = p1.m_x + m_blendWeight * (p2.m_x - p1.m_x);
		tr.m_y = p1.m_y + m_blendWeight * (p2.m_y - p1.m_y);
		tr.m_z = p1.m_z + m_blendWeight * (p2.m_z - p1.m_z);

		// interpolation of scale
		Vector3D& sc = endPose.m_scaling;
		Vector3D& s1 = pose1.m_scaling;
		Vector3D& s2 = pose2.m_scaling;
		sc.m_x = s1.m_x + m_blendWeight * (s2.m_x - s1.m_x);
		sc.m_y = s1.m_y + m_blendWeight * (s2.m_y - s1.m_y);
		sc.m_z = s1.m_z + m_blendWeight * (s2.m_z - s1.m_z);
	}
}