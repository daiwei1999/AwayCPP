#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * DirectionalLightNode is a space partitioning leaf node that contains a DirectionalLight object.
 */
class DirectionalLightNode : public EntityNode
{
public:
	DirectionalLightNode(DirectionalLight* light);

	DirectionalLight* getLight() { return m_light; }
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	DirectionalLight* m_light;
};

AWAY_NAMESPACE_END