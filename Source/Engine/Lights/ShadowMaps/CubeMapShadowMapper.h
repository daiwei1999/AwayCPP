#pragma once

#include "Common.h"
#include "ShadowMapperBase.h"
#include "Camera3D.h"

AWAY_NAMESPACE_BEGIN

class CubeMapShadowMapper : public ShadowMapperBase
{
public:
	CubeMapShadowMapper();

protected:
	TextureProxyBase* createDepthTexture() override;
	void updateDepthProjection(Camera3D* viewCamera) override;
	void drawDepthMap(TextureBase* target, Scene3D* scene, DepthRenderer* renderer) override;

private:
	void setCameraOrientation(Camera3D& camera, float rotationX, float rotationY, float rotationZ);

private:
	Camera3D m_depthCameras[6];
	bool m_needsRender[6];
};

AWAY_NAMESPACE_END