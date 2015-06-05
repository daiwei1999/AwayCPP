#pragma once

#include "Common.h"
#include "NodeBase.h"

AWAY_NAMESPACE_BEGIN

class QuadtreeNode : public NodeBase
{
public:
	QuadtreeNode(int maxDepth = 5, float size = 10000, float height = 1000000, float centerX = 0, float centerZ = 0, int depth = 0);

	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	NodeBase* findPartitionForEntity(Entity* entity) override;

private:
	QuadtreeNode* findPartitionForBounds(float minX, float minZ, float maxX, float maxZ);

private:
	float m_centerX;
	float m_centerZ;
	float m_height;
	float m_halfExtentXZ;
	float m_halfExtentY;
	bool m_leaf;

	QuadtreeNode* m_rightFar;
	QuadtreeNode* m_leftFar;
	QuadtreeNode* m_rightNear;
	QuadtreeNode* m_leftNear;
};

AWAY_NAMESPACE_END