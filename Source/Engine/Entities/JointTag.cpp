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
		SkeletonAnimator* animator = static_cast<SkeletonAnimator*>(static_cast<Mesh*>(m_parent)->getAnimator());
		m_sceneTransform.copyFrom(animator->getGlobalMatrices()[m_jointIndex]);
		m_sceneTransform.append(m_parent->getSceneTransform());
		m_sceneTransform.prepend(getTransform());
	}
	m_sceneTransformDirty = false;
}