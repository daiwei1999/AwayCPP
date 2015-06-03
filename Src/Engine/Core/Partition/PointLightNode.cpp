#include "PointLightNode.h"
#include "PointLight.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

PointLightNode::PointLightNode(PointLight* light) : EntityNode(light)
{
	m_light = light;
}

void PointLightNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);
		traverser->applyPointLight(m_light);
	}
}