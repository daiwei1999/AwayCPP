#include "Skeleton.h"

USING_AWAY_NAMESPACE

void Skeleton::buildIndexTable()
{
	int numJoints = m_joints.size();
	std::map<int, std::vector<int>> depthIndicesMap;
	for (int i = 0; i < numJoints; i++)
	{
		int depth = getJointDepth(m_joints[i]);
		auto iter = depthIndicesMap.find(depth);
		if (iter == depthIndicesMap.end())
			depthIndicesMap[depth] = { i };
		else
			iter->second.push_back(i);
	}

	for (auto& item : depthIndicesMap)
	{
		auto& indices = item.second;
		for (auto index : indices)
			m_indexTable.push_back(index);
	}
}

SkeletonJoint* Skeleton::getJointByName(const std::string& name)
{
	int jointIndex = getJointIndexByName(name);
	return jointIndex == -1 ? nullptr : m_joints[jointIndex];
}

int Skeleton::getJointIndexByName(const std::string& name)
{
	int jointIndex = 0;
	for (auto joint : m_joints)
	{
		if (joint->m_name == name)
			return jointIndex;

		jointIndex++;
	}
	return -1;
}

int Skeleton::getJointDepth(SkeletonJoint* joint)
{
	int parentIndex = joint->m_parentIndex;
	return parentIndex < 0 ? 0 : (getJointDepth(m_joints[parentIndex]) + 1);
}