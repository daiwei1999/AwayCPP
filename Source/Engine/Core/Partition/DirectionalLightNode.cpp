#include "DirectionalLightNode.h"
#include "DirectionalLight.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

DirectionalLightNode::DirectionalLightNode(DirectionalLight* light) : EntityNode(light)
{
	m_light = light;
}

void DirectionalLightNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);
		traverser->applyDirectionalLight(m_light);
	}
}