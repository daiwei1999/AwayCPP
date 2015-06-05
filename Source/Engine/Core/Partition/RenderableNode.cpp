#include "RenderableNode.h"
#include "Entity.h"
#include "IRenderable.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

RenderableNode::RenderableNode(IRenderable* renderable) : EntityNode(dynamic_cast<Entity*>(renderable))
{
	m_renderable = renderable;
}

void RenderableNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);
		traverser->applyRenderable(m_renderable);
	}
}