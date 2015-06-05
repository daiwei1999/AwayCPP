#pragma once

#include "Common.h"
#include "Quaternion.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * Contains transformation data for a skeleton joint, used for skeleton animation.
 */
class JointPose
{
public:
	Matrix3D& toMatrix3D(Matrix3D& target);

public:
	Quaternion m_orientation; // The rotation of the pose stored as a quaternion
	Vector3D m_translation; // The translation of the pose
};

/**
 * A collection of pose objects, determining the pose for an entire skeleton.
 * The <code>jointPoses</code> vector object corresponds to a skeleton's <code>joints</code> vector object, however, there is no
 * reference to a skeleton's instance, since several skeletons can be influenced by the same pose (eg: animation
 * clips are added to any animator with a valid skeleton)
 */
class SkeletonPose
{
public:
	int getJointPoseCount() { return m_jointPoses.size(); }

public:
	std::vector<JointPose> m_jointPoses;
};

AWAY_NAMESPACE_END