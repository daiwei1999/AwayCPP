#include "Scene3D.h"
#include "Partition3D.h"
#include "NodeBase.h"
#include "PartitionTraverser.h"
#include "Scene3DEvent.h"
#include "Entity.h"

USING_AWAY_NAMESPACE

Scene3D::Scene3D()
{
	m_sceneGraphRoot.setScene(this);
	m_sceneGraphRoot.m_isRoot = true;
	m_sceneGraphRoot.setPartition(new Partition3D(new NodeBase()));
}

void Scene3D::traversePartitions(PartitionTraverser* traverser)
{
	traverser->m_scene = this;

	for (auto partition : m_partitions)
		partition->traverse(traverser);
}

void Scene3D::setPartition(Partition3D* value)
{
	m_sceneGraphRoot.setPartition(value);

	// dispatch Scene3DEvent.PARTITION_CHANGED event
	Scene3DEvent event(Scene3DEvent::PARTITION_CHANGED, &m_sceneGraphRoot);
	dispatchEvent(&event);
}

void Scene3D::registerEntity(Entity* entity)
{
	Partition3D* partition = entity->getImplicitPartition();
	addPartitionUnique(partition);
	partition->markForUpdate(entity);
}

void Scene3D::unregisterEntity(Entity* entity)
{
	entity->getImplicitPartition()->removeEntity(entity);
}

void Scene3D::invalidateEntityBounds(Entity* entity)
{
	entity->getImplicitPartition()->markForUpdate(entity);
}

void Scene3D::registerPartition(Entity* entity)
{
	addPartitionUnique(entity->getImplicitPartition());
}

void Scene3D::unregisterPartition(Entity* entity)
{
	entity->getImplicitPartition()->removeEntity(entity);
}

void Scene3D::addPartitionUnique(Partition3D* partition)
{
	if (std::find(m_partitions.begin(), m_partitions.end(), partition) == m_partitions.end())
		m_partitions.push_back(partition);
}