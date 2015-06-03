#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * PointLightNode is a space partitioning leaf node that contains a PointLight object.
 */
class PointLightNode : public EntityNode
{
public:
	PointLightNode(PointLight* light);

	PointLight* getLight() { return m_light; }
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	PointLight* m_light;
};

AWAY_NAMESPACE_END