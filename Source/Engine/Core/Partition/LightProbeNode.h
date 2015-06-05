#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * LightProbeNode is a space partitioning leaf node that contains a LightProbe object.
 */
class LightProbeNode : public EntityNode
{
public:
	LightProbeNode(LightProbe* light);

	LightProbe* getLight() { return m_light; }
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	LightProbe* m_light;
};

AWAY_NAMESPACE_END