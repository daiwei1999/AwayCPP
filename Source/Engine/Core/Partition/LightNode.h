#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * LightNode is a space partitioning leaf node that contains a LightBase object. Used for lights that are not of default supported type.
 */
class LightNode : public EntityNode
{
public:
	LightNode(LightBase* light);

	LightBase* getLight() { return m_light; }
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	LightBase* m_light;
};

AWAY_NAMESPACE_END