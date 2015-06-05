#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * SkyBoxNode is a space partitioning leaf node that contains a SkyBox object.
 */
class SkyBoxNode : public EntityNode
{
public:
	SkyBoxNode(SkyBox* skyBox);

	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	void acceptTraverser(PartitionTraverser* traverser) override;

private:
	SkyBox* m_skyBox;
};

AWAY_NAMESPACE_END