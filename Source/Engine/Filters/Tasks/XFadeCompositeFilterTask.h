#pragma once

#include "Common.h"
#include "FilterTaskBase.h"

AWAY_NAMESPACE_BEGIN

class XFadeCompositeFilterTask : public FilterTaskBase
{
public:
	XFadeCompositeFilterTask(float amount);

	float getAmount() { return m_data[0]; }
	void setAmount(float value) { m_data[0] = value; }
	TextureBase* getOverlayTexture() { return m_overlayTexture; }
	void setOverlayTexture(TextureBase* value) { m_overlayTexture = value; }

	void activate(IContext* context, Camera3D* camera, Texture* depthTexture) override;
	void deactivate(IContext* context) override;

protected:
	void getFragmentCode(Shader& fragmentShader) override;

private:
	float m_data[4];
	TextureBase* m_overlayTexture;
};

AWAY_NAMESPACE_END