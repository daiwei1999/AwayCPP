#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * A value object representing a single joint in a skeleton object.
 */
class SkeletonJoint
{
public:
	std::string m_name;
	int m_parentIndex = -1;
	float m_inverseBindPose[12];
};

/**
 * A Skeleton object is a hierarchical grouping of joint objects that can be used for skeletal animation.
 */
class Skeleton
{
public:
	void buildIndexTable();
	SkeletonJoint* getJointByName(const std::string& name);
	int getJointIndexByName(const std::string& name);

private:
	int getJointDepth(SkeletonJoint* joint);

public:
	int m_numSkinningJoints;
	std::vector<int> m_indexTable;
	std::vector<SkeletonJoint*> m_joints;
};

AWAY_NAMESPACE_END