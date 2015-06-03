#pragma once

#include "Common.h"
#include "EventDispatcher.h"
#include "ObjectContainer3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * The Scene3D class represents an independent 3D scene in which 3D objects can be created and manipulated.
 * Multiple Scene3D instances can be created in the same SWF file.
 *
 * Scene management happens through the scene graph, which is exposed using addChild and removeChild methods.
 * Internally, the Scene3D object also manages any space partition objects that have been assigned to objects in
 * the scene graph, of which there is at least 1.
 */
class Scene3D : public EventDispatcher
{
public:
	Scene3D();

	/**
	 * Sends a PartitionTraverser object down the scene partitions
	 */
	void traversePartitions(PartitionTraverser* traverser);

	Partition3D* getPartition() { return m_sceneGraphRoot.getPartition(); }
	void setPartition(Partition3D* value);

	bool contains(ObjectContainer3D* child) { return m_sceneGraphRoot.contains(child); }
	ObjectContainer3D* addChild(ObjectContainer3D* child) { return m_sceneGraphRoot.addChild(child); }
	void removeChild(ObjectContainer3D* child) { m_sceneGraphRoot.removeChild(child); }
	void removeChildAt(int index) { m_sceneGraphRoot.removeChildAt(index); }
	ObjectContainer3D* getChildAt(int index) { return m_sceneGraphRoot.getChildAt(index); }
	int getChildCount() { return m_sceneGraphRoot.getChildCount(); }

	void registerEntity(Entity* entity);
	void unregisterEntity(Entity* entity);

	void invalidateEntityBounds(Entity* entity);
	void registerPartition(Entity* entity);
	void unregisterPartition(Entity* entity);

private:
	void addPartitionUnique(Partition3D* partition);

private:
	ObjectContainer3D m_sceneGraphRoot;
	std::vector<Partition3D*> m_partitions;
};

AWAY_NAMESPACE_END