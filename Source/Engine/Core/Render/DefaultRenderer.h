#pragma once

#include "Common.h"
#include "RendererBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The DefaultRenderer class provides the default rendering method. It renders the scene graph objects using the
 * materials assigned to them.
 */
class DefaultRenderer : public RendererBase
{
public:
	DefaultRenderer();

	void setContext(IContext* value) override;

protected:
	void executeRender(EntityCollector* entityCollector, TextureBase* target = nullptr, Rectangle<int>* scissorRect = nullptr, int surfaceSelector = 0) override;
	void draw(EntityCollector* entityCollector, TextureBase* target) override;

private:
	void updateLights(EntityCollector* entityCollector);
	void drawSkyBox(EntityCollector* entityCollector);
	void updateSkyBoxProjection(Camera3D* camera);
	void drawRenderables(RenderableListItem* item, EntityCollector* entityCollector, int which);

private:
	MaterialBase* m_activeMaterial;
	DepthRenderer* m_depthRenderer;
	DepthRenderer* m_distanceRenderer;
	Matrix3D m_skyboxProjection;
};

AWAY_NAMESPACE_END