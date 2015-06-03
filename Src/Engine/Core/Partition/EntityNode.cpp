#include "EntityNode.h"
#include "Entity.h"
#include "BoundingVolumeBase.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

EntityNode::EntityNode(Entity* entity)
{
	m_entity = entity;
	m_numEntities = 1;
}

void EntityNode::removeFromParent()
{
	if (m_parent)
	{
		m_parent->removeNode(this);
		m_parent = nullptr;
	}
}

bool EntityNode::isInFrustum(PlaneVector* planes, int numPlanes)
{
	if (!m_entity->isVisible())
		return false;

	return m_entity->getWorldBounds()->isInFrustum(planes, numPlanes);
}

bool EntityNode::isIntersectingRay(Vector3D* rayPosition, Vector3D* rayDirection)
{
	if (!m_entity->isVisible())
		return false;

	return m_entity->isIntersectingRay(rayPosition, rayDirection);
}

void EntityNode::acceptTraverser(PartitionTraverser* traverser)
{
	traverser->applyEntity(m_entity);
}