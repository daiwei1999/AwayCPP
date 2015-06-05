#pragma once

#include "Common.h"
#include "EntityNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * CameraNode is a space partitioning leaf node that contains a Camera3D object.
 */
class CameraNode : public EntityNode
{
public:
	CameraNode(Camera3D* camera);
	void acceptTraverser(PartitionTraverser* traverser) override;
};

AWAY_NAMESPACE_END