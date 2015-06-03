#include "SkeletonPose.h"
#include "Matrix3D.h"

USING_AWAY_NAMESPACE

Matrix3D& JointPose::toMatrix3D(Matrix3D& target)
{
	m_orientation.toMatrix3D(target);
	target.appendTranslation(m_translation.m_x, m_translation.m_y, m_translation.m_z);
	return target;
}