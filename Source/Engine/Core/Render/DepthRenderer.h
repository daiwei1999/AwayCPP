#pragma once

#include "Common.h"
#include "RendererBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The DepthRenderer class renders 32-bit depth information encoded as RGBA
 */
class DepthRenderer : public RendererBase
{
public:
	DepthRenderer(bool renderBlended = false, bool distanceBased = false);

	bool getDisableColor() { return m_disableColor; }
	void setDisableColor(bool value) { m_disableColor = value; }
	void setBackgroundR(float value) override {}
	void setBackgroundG(float value) override {}
	void setBackgroundB(float value) override {}

protected:
	void draw(EntityCollector* entityCollector, TextureBase* target) override;

private:
	void drawRenderables(RenderableListItem* item, EntityCollector* entityCollector);

private:
	MaterialBase* m_activeMaterial;
	bool m_renderBlended;
	bool m_distanceBased;
	bool m_disableColor;
};

AWAY_NAMESPACE_END