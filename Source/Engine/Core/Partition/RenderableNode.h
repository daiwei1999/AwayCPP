#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * RenderableNode is a space partitioning leaf node that contains any Entity that is itself a IRenderable
 * object. This excludes Mesh (since the renderable objects are its SubMesh children).
 */
class RenderableNode : public EntityNode
{
public:
	RenderableNode(IRenderable* renderable);
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	IRenderable* m_renderable;
};

AWAY_NAMESPACE_END