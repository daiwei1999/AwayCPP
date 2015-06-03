#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * PartitionTraverser is a hierarchical visitor pattern that traverses through a Partition3D data structure.
 */
class PartitionTraverser
{
public:
	PartitionTraverser();

	virtual bool enterNode(NodeBase* node) = 0;
	virtual void applySkyBox(IRenderable* renderable) = 0;
	virtual void applyRenderable(IRenderable* renderable) = 0;
	virtual void applyUnknownLight(LightBase* light) = 0;
	virtual void applyDirectionalLight(DirectionalLight* light) = 0;
	virtual void applyPointLight(PointLight* light) = 0;
	virtual void applyLightProbe(LightProbe* light) = 0;
	virtual void applyEntity(Entity* entity) = 0;

	Vector3D* getEntryPoint() { return m_entryPoint; }

public:
	Scene3D* m_scene;
	static unsigned int m_collectionMark;

protected:
	Vector3D* m_entryPoint;
};

AWAY_NAMESPACE_END