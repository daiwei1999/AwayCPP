#include "QuadtreeNode.h"
#include "Entity.h"
#include "BoundingVolumeBase.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

QuadtreeNode::QuadtreeNode(int maxDepth, float size, float height, float centerX, float centerZ, int depth)
{
	m_centerX = centerX;
	m_centerZ = centerZ;
	m_height = height;
	m_halfExtentXZ = size * .5f;
	m_halfExtentY = height * .5f;
	m_leaf = depth == maxDepth;

	if (!m_leaf)
	{
		float hhs = m_halfExtentXZ * .5f;
		addNode(m_leftNear = new QuadtreeNode(maxDepth, m_halfExtentXZ, height, centerX - hhs, centerZ - hhs, depth + 1));
		addNode(m_rightNear = new QuadtreeNode(maxDepth, m_halfExtentXZ, height, centerX + hhs, centerZ - hhs, depth + 1));
		addNode(m_leftFar = new QuadtreeNode(maxDepth, m_halfExtentXZ, height, centerX - hhs, centerZ + hhs, depth + 1));
		addNode(m_rightFar = new QuadtreeNode(maxDepth, m_halfExtentXZ, height, centerX + hhs, centerZ + hhs, depth + 1));
	}
}

bool QuadtreeNode::isInFrustum(PlaneVector* planes, int numPlanes)
{
	float flippedExtentX, flippedExtentY, flippedExtentZ;
	for (int i = 0; i < numPlanes; i++)
	{
		Plane3D& plane = (*planes)[i];
		flippedExtentX = plane.m_a < 0 ? -m_halfExtentXZ : m_halfExtentXZ;
		flippedExtentY = plane.m_b < 0 ? -m_halfExtentY : m_halfExtentY;
		flippedExtentZ = plane.m_c < 0 ? -m_halfExtentXZ : m_halfExtentXZ;
		if (plane.m_a * (m_centerX + flippedExtentX) + plane.m_b * flippedExtentY + plane.m_c * (m_centerZ + flippedExtentZ) - plane.m_d < 0)
			return false;
	}

	return true;
}

NodeBase* QuadtreeNode::findPartitionForEntity(Entity* entity)
{
	BoundingVolumeBase* bounds = entity->getWorldBounds();
	Vector3D* min = bounds->getMin();
	Vector3D* max = bounds->getMax();
	return findPartitionForBounds(min->m_x, min->m_z, max->m_x, max->m_z);
}

QuadtreeNode* QuadtreeNode::findPartitionForBounds(float minX, float minZ, float maxX, float maxZ)
{
	if (m_leaf)
		return this;

	bool right = maxX > m_centerX;
	bool left = minX < m_centerX;
	bool far = maxZ > m_centerZ;
	bool near = minZ < m_centerZ;
	if (left && right)
		return this;

	if (near)
	{
		if (far)
			return this;

		if (left)
			return m_leftNear->findPartitionForBounds(minX, minZ, maxX, maxZ);
		else
			return m_rightNear->findPartitionForBounds(minX, minZ, maxX, maxZ);
	}
	else
	{
		if (left)
			return m_leftFar->findPartitionForBounds(minX, minZ, maxX, maxZ);
		else
			return m_rightFar->findPartitionForBounds(minX, minZ, maxX, maxZ);
	}
}