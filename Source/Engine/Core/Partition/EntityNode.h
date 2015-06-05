#pragma once

#include "Common.h"
#include "NodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The EntityNode class provides an abstract base class for leaf nodes in a partition tree, containing
 * entities to be fed to the EntityCollector traverser.
 * The concrete subtype of Entity is responsible for creating a matching subtype of EntityNode.
 */
class EntityNode : public NodeBase
{
public:
	EntityNode(Entity* entity);

	Entity* getEntity() { return m_entity; }
	void removeFromParent();

	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	bool isIntersectingRay(Vector3D* rayPosition, Vector3D* rayDirection) override;
	void acceptTraverser(PartitionTraverser* traverser) override;

public:
	EntityNode* m_updateQueueNext;

private:
	Entity* m_entity;
};

AWAY_NAMESPACE_END