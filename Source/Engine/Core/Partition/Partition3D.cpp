#include "Partition3D.h"
#include "NodeBase.h"
#include "Entity.h"
#include "EntityNode.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

Partition3D::Partition3D(NodeBase* rootNode)
{
	m_rootNode = rootNode;
	m_updateQueue = nullptr;
	m_updatesMade = false;
}

bool Partition3D::getShowBounds()
{
	return m_rootNode->getShowBounds();
}

void Partition3D::setShowBounds(bool value)
{
	m_rootNode->setShowBounds(value);
}

void Partition3D::traverse(PartitionTraverser* traverser)
{
	if (m_updatesMade)
		updateEntities();

	++PartitionTraverser::m_collectionMark;
	m_rootNode->acceptTraverser(traverser);
}

void Partition3D::markForUpdate(Entity* entity)
{
	EntityNode* node = entity->getEntityPartitionNode();
	EntityNode* t = m_updateQueue;
	while (t)
	{
		if (node == t)
			return;

		t = t->m_updateQueueNext;
	}

	node->m_updateQueueNext = m_updateQueue;
	m_updateQueue = node;
	m_updatesMade = true;
}

void Partition3D::removeEntity(Entity* entity)
{
	EntityNode* node = entity->getEntityPartitionNode();
	node->removeFromParent();

	// remove from update list if it's in
	if (node == m_updateQueue)
	{
		m_updateQueue = node->m_updateQueueNext;
	}
	else
	{
		EntityNode* t = m_updateQueue;
		while (t && t->m_updateQueueNext != node)
			t = t->m_updateQueueNext;

		if (t)
			t->m_updateQueueNext = node->m_updateQueueNext;
	}

	node->m_updateQueueNext = nullptr;

	// any updates have been made undone
	if (!m_updateQueue)
		m_updatesMade = false;
}

void Partition3D::updateEntities()
{
	EntityNode* t;
	NodeBase* targetNode;
	EntityNode* node = m_updateQueue;

	// clear updateQueue early to allow for newly marked entity updates
	m_updateQueue = nullptr;
	m_updatesMade = false;

	do 
	{
		targetNode = m_rootNode->findPartitionForEntity(node->getEntity());

		// if changed, find and attach the mesh node to the best suited partition node
		if (node->getParent() != targetNode)
		{
			if (node)
				node->removeFromParent();

			targetNode->addNode(node);
		}

		t = node->m_updateQueueNext;
		node->m_updateQueueNext = nullptr;

		//call an internal update on the entity to fire any attached logic
		node->getEntity()->internalUpdate();

	} while ((node = t) != nullptr);
}