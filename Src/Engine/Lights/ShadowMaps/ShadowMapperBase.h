#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ShadowMapperBase
{
public:
	ShadowMapperBase();
	virtual ~ShadowMapperBase();

	bool getAutoUpdateShadows() { return m_autoUpdateShadows; }
	void setAutoUpdateShadows(bool value) { m_autoUpdateShadows = value; }
	LightBase* getLight() { return m_light; }
	void setLight(LightBase* value) { m_light = value; }
	TextureProxyBase* getDepthMap() { return m_depthMap; }
	void setDepthMap(TextureProxyBase* depthMap);
	int getDepthMapSize() { return m_depthMapSize; }
	void setDepthMapSize(int value);

	ShadowCasterCollector* getCasterCollector() { return m_casterCollector; }
	bool isShadowsInvalid() { return m_shadowsInvalid; }
	void updateShadows() { m_shadowsInvalid = true; }
	void renderDepthMap(IContext* context, EntityCollector* entityCollector, DepthRenderer* renderer);

protected:
	virtual TextureProxyBase* createDepthTexture();
	virtual void updateDepthProjection(Camera3D* viewCamera) = 0;
	virtual void drawDepthMap(TextureBase* target, Scene3D* scene, DepthRenderer* renderer) = 0;

protected:
	ShadowCasterCollector* m_casterCollector;
	int m_depthMapSize;
	LightBase* m_light;

private:
	TextureProxyBase* m_depthMap;
	bool m_explicitDepthMap;
	bool m_autoUpdateShadows;
	bool m_shadowsInvalid;
};

AWAY_NAMESPACE_END