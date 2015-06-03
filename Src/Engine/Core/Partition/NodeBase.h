#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * The NodeBase class is an abstract base class for any type of space partition tree node. The concrete
 * subtype will control the creation of its child nodes, which are necessarily of the same type. The exception is
 * the creation of leaf entity nodes, which is handled by the Partition3D class.
 */
class NodeBase
{
public:
	NodeBase();

	bool getShowBounds() { return m_debugPrimitive != nullptr; }
	void setShowBounds(bool value);

	NodeBase* getParent() { return m_parent; }
	int getEntityCount() { return m_numEntities; }

	void addNode(NodeBase* node);
	void removeNode(NodeBase* node);

	unsigned int getCollectionMark() { return m_collectionMark; }
	void setCollectionMark(unsigned int value) { m_collectionMark = value; }

	virtual bool isInFrustum(PlaneVector* planes, int numPlanes);
	virtual bool isIntersectingRay(Vector3D* rayPosition, Vector3D* rayDirection);
	virtual NodeBase* findPartitionForEntity(Entity* entity);
	virtual void acceptTraverser(PartitionTraverser* traverser);

protected:
	virtual WireframePrimitiveBase* createDebugBounds();
	void updateNumEntities(int value);

protected:
	NodeBase* m_parent;
	std::vector<NodeBase*> m_childNodes;
	int m_numChildNodes;
	int m_numEntities;
	unsigned int m_collectionMark;
	WireframePrimitiveBase* m_debugPrimitive;
};

AWAY_NAMESPACE_END