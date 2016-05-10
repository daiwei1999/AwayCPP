#include "SkeletonPose.h"
#include "Matrix3D.h"

USING_AWAY_NAMESPACE

Matrix3D& JointPose::toMatrix3D(Matrix3D& target)
{
	Vector3D rotation(m_orientation.m_x, m_orientation.m_y, m_orientation.m_z, m_orientation.m_w);
	target.recompose(m_translation, rotation, m_scaling, Orientation3D::QUATERNION);
	return target;
}