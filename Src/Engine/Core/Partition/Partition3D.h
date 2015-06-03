#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * Partition3D is the core of a space partition system. The space partition system typically subdivides the 3D scene
 * hierarchically into a number of non-overlapping subspaces, forming a tree data structure. This is used to more
 * efficiently perform frustum culling, potential visibility determination and collision detection.
 */
class Partition3D
{
public:
	Partition3D(NodeBase* rootNode);

	bool getShowBounds();
	void setShowBounds(bool value);

	void traverse(PartitionTraverser* traverser);
	void markForUpdate(Entity* entity);
	void removeEntity(Entity* entity);

private:
	void updateEntities();

private:
	NodeBase* m_rootNode;
	EntityNode* m_updateQueue;
	bool m_updatesMade;
};

AWAY_NAMESPACE_END