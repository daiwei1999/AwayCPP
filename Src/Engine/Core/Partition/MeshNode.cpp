#include "MeshNode.h"
#include "Mesh.h"
#include "SubMesh.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

MeshNode::MeshNode(Mesh* mesh) : EntityNode(mesh)
{
	m_mesh = mesh;
}

void MeshNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);

		for (auto subMesh : m_mesh->getSubMeshes())
			traverser->applyRenderable(subMesh);
	}
}