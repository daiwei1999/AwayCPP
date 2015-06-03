#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * MeshNode is a space partitioning leaf node that contains a Mesh object.
 */
class MeshNode : public EntityNode
{
public:
	MeshNode(Mesh* mesh);

	Mesh* getMesh() { return m_mesh; }
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	Mesh* m_mesh;
};

AWAY_NAMESPACE_END