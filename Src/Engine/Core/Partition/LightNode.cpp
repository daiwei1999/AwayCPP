#include "LightNode.h"
#include "LightBase.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

LightNode::LightNode(LightBase* light) : EntityNode(light)
{
	m_light = light;
}

void LightNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);
		traverser->applyUnknownLight(m_light);
	}
}