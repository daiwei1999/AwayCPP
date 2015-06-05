#pragma once

#include "Common.h"
#include "PartitionTraverser.h"
#include "EntityListItemPool.h"
#include "RenderableListItemPool.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * The EntityCollector class is a traverser for scene partitions that collects all scene graph entities that are
 * considered potentially visible.
 */
class EntityCollector : public PartitionTraverser
{
public:
	EntityCollector();

	Camera3D* getCamera() { return m_camera; }
	void setCamera(Camera3D* value) { m_camera = value; }

	PlaneVector* getCullPlanes() { return m_customCullPlanes; }
	void setCullPlanes(PlaneVector* value) { m_customCullPlanes = value; }

	int getMouseEnabledCount() { return m_numMouseEnableds; }
	IRenderable* getSkyBox() { return m_skyBox; }
	EntityListItem* getEntityHead() { return m_entityHead; }
	LightVector& getLights() { return m_lights; }
	DirectionalLightVector& getDirectionalLights() { return m_directionalLights; }
	PointLightVector& getPointLights() { return m_pointLights; }
	LightProbeVector& getLightProbes() { return m_lightProbes; }
	RenderableListItem* getOpaqueRenderableHead() { return m_opaqueRenderableHead; }
	void setOpaqueRenderableHead(RenderableListItem* value) { m_opaqueRenderableHead = value; }
	RenderableListItem* getBlendedRenderableHead() { return m_blendedRenderableHead; }
	void setBlendedRenderableHead(RenderableListItem* value) { m_blendedRenderableHead = value; }

	void clear();
	bool enterNode(NodeBase* node) override;
	void applySkyBox(IRenderable* renderable) override { m_skyBox = renderable; }
	void applyRenderable(IRenderable* renderable) override;
	void applyUnknownLight(LightBase* light) override;
	void applyDirectionalLight(DirectionalLight* light) override;
	void applyPointLight(PointLight* light) override;
	void applyLightProbe(LightProbe* light) override;
	void applyEntity(Entity* entity) override;

protected:
	IRenderable* m_skyBox;
	RenderableListItem* m_opaqueRenderableHead;
	RenderableListItem* m_blendedRenderableHead;
	RenderableListItemPool m_renderableListItemPool;
	EntityListItemPool m_entityListItemPool;
	LightVector m_lights;
	int m_numLights;
	int m_numTriangles;
	int m_numMouseEnableds;
	Camera3D* m_camera;
	Vector3D m_cameraForward;

private:
	EntityListItem* m_entityHead;
	DirectionalLightVector m_directionalLights;
	PointLightVector m_pointLights;
	LightProbeVector m_lightProbes;
	int m_numDirectionalLights;
	int m_numPointLights;
	int m_numLightProbes;
	PlaneVector* m_cullPlanes;
	PlaneVector* m_customCullPlanes;
	int m_numCullPlanes;
};

AWAY_NAMESPACE_END