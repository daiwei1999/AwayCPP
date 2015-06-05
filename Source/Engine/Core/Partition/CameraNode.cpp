#include "CameraNode.h"
#include "Camera3D.h"

USING_AWAY_NAMESPACE

CameraNode::CameraNode(Camera3D* camera) : EntityNode(camera)
{

}

void CameraNode::acceptTraverser(PartitionTraverser* traverser)
{
	// todo: dead end for now, if it has a debug mesh, then sure accept that
}