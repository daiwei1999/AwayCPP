#pragma once

#include "Common.h"
#include "NodeBase.h"

AWAY_NAMESPACE_BEGIN

class OctreeNode : public NodeBase
{
public:
	OctreeNode(int maxDepth = 5, float size = 10000, float centerX = 0, float centerY = 0, float centerZ = 0, int depth = 0);

	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	NodeBase* findPartitionForEntity(Entity* entity) override;

protected:
	WireframePrimitiveBase* createDebugBounds() override;

private:
	OctreeNode* findPartitionForBounds(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);

private:
	float m_halfExtent;
	float m_centerX;
	float m_centerY;
	float m_centerZ;
	float m_minX;
	float m_minY;
	float m_minZ;
	float m_maxX;
	float m_maxY;
	float m_maxZ;
	float m_quadSize;
	bool m_leaf;

	OctreeNode* m_rightTopFar;
	OctreeNode* m_leftTopFar;
	OctreeNode* m_rightBottomFar;
	OctreeNode* m_leftBottomFar;
	OctreeNode* m_rightTopNear;
	OctreeNode* m_leftTopNear;
	OctreeNode* m_rightBottomNear;
	OctreeNode* m_leftBottomNear;
};

AWAY_NAMESPACE_END