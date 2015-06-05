#include "LightProbeNode.h"
#include "LightProbe.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

LightProbeNode::LightProbeNode(LightProbe* light) : EntityNode(light)
{
	m_light = light;
}

void LightProbeNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);
		traverser->applyLightProbe(m_light);
	}
}