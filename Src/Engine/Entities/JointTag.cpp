#include "JointTag.h"
#include "Mesh.h"
#include "SkeletonAnimator.h"

USING_AWAY_NAMESPACE

JointTag::JointTag(int jointIndex)
{
	m_jointIndex = jointIndex;
}

void JointTag::updateSceneTransform()
{
	if (m_parent)
	{
		Matrix3D matrix;
		SkeletonAnimator* animator = (SkeletonAnimator*)((Mesh*)m_parent)->getAnimator();
		m_sceneTransform.copyFrom(animator->getGlobalPose().m_jointPoses[m_jointIndex].toMatrix3D(matrix));
		m_sceneTransform.append(m_parent->getSceneTransform());
		m_sceneTransform.prepend(getTransform());
	}
	m_sceneTransformDirty = false;
}