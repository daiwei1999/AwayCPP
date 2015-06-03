#include "OctreeNode.h"
#include "Entity.h"
#include "WireframeCube.h"
#include "BoundingVolumeBase.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

OctreeNode::OctreeNode(int maxDepth, float size, float centerX, float centerY, float centerZ, int depth)
{
	m_halfExtent = size * .5f;
	m_centerX = centerX;
	m_centerY = centerY;
	m_centerZ = centerZ;
	m_quadSize = size;
	m_minX = centerX - m_halfExtent;
	m_minY = centerY - m_halfExtent;
	m_minZ = centerZ - m_halfExtent;
	m_maxX = centerX + m_halfExtent;
	m_maxY = centerY + m_halfExtent;
	m_maxZ = centerZ + m_halfExtent;
	m_leaf = depth == maxDepth;

	if (!m_leaf)
	{
		float hhs = m_halfExtent * .5f;
		addNode(m_leftTopNear = new OctreeNode(maxDepth, m_halfExtent, centerX - hhs, centerY + hhs, centerZ - hhs, depth + 1));
		addNode(m_rightTopNear = new OctreeNode(maxDepth, m_halfExtent, centerX + hhs, centerY + hhs, centerZ - hhs, depth + 1));
		addNode(m_leftBottomNear = new OctreeNode(maxDepth, m_halfExtent, centerX - hhs, centerY - hhs, centerZ - hhs, depth + 1));
		addNode(m_rightBottomNear = new OctreeNode(maxDepth, m_halfExtent, centerX + hhs, centerY - hhs, centerZ - hhs, depth + 1));
		addNode(m_leftTopFar = new OctreeNode(maxDepth, m_halfExtent, centerX - hhs, centerY + hhs, centerZ + hhs, depth + 1));
		addNode(m_rightTopFar = new OctreeNode(maxDepth, m_halfExtent, centerX + hhs, centerY + hhs, centerZ + hhs, depth + 1));
		addNode(m_leftBottomFar = new OctreeNode(maxDepth, m_halfExtent, centerX - hhs, centerY - hhs, centerZ + hhs, depth + 1));
		addNode(m_rightBottomFar = new OctreeNode(maxDepth, m_halfExtent, centerX + hhs, centerY - hhs, centerZ + hhs, depth + 1));
	}
}

bool OctreeNode::isInFrustum(PlaneVector* planes, int numPlanes)
{
	float flippedExtentX, flippedExtentY, flippedExtentZ;
	for (int i = 0; i < numPlanes; i++)
	{
		Plane3D& plane = (*planes)[i];
		flippedExtentX = plane.m_a < 0 ? -m_halfExtent : m_halfExtent;
		flippedExtentY = plane.m_b < 0 ? -m_halfExtent : m_halfExtent;
		flippedExtentZ = plane.m_c < 0 ? -m_halfExtent : m_halfExtent;
		if (plane.m_a * (m_centerX + flippedExtentX) + plane.m_b * (m_centerY + flippedExtentY) + plane.m_c * (m_centerZ + flippedExtentZ) - plane.m_d < 0)
			return false;
	}

	return true;
}

NodeBase* OctreeNode::findPartitionForEntity(Entity* entity)
{
	BoundingVolumeBase* bounds = entity->getWorldBounds();
	Vector3D* min = bounds->getMin();
	Vector3D* max = bounds->getMax();
	return findPartitionForBounds(min->m_x, min->m_y, min->m_z, max->m_x, max->m_y, max->m_z);
}

WireframePrimitiveBase* OctreeNode::createDebugBounds()
{
	WireframeCube* cube = new WireframeCube(m_quadSize, m_quadSize, m_quadSize);
	cube->setX(m_centerX);
	cube->setY(m_centerY);
	cube->setZ(m_centerZ);
	return cube;
}

OctreeNode* OctreeNode::findPartitionForBounds(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	if (m_leaf)
		return this;

	bool right = maxX > m_centerX;
	bool left = minX < m_centerX;
	bool top = maxY > m_centerY;
	bool bottom = minY < m_centerY;
	bool far = maxZ > m_centerZ;
	bool near = minZ < m_centerZ;
	if ((left && right) || (far && near))
		return this;

	if (top)
	{
		if (bottom)
			return this;

		if (near)
		{
			if (left)
				return m_leftTopNear->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
			else
				return m_rightTopNear->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
		}
		else
		{
			if (left)
				return m_leftTopFar->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
			else
				return m_rightTopFar->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
		}
	}
	else
	{
		if (near)
		{
			if (left)
				return m_leftBottomNear->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
			else
				return m_rightBottomNear->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
		}
		else
		{
			if (left)
				return m_leftBottomFar->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
			else
				return m_rightBottomFar->findPartitionForBounds(minX, minY, minZ, maxX, maxY, maxZ);
		}
	}
}