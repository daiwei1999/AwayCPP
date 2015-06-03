#pragma once

#include "Common.h"
#include "ShadowMapperBase.h"
#include "FreeMatrixLens.h"

AWAY_NAMESPACE_BEGIN

class DirectionalShadowMapper : public ShadowMapperBase
{
public:
	DirectionalShadowMapper();
	~DirectionalShadowMapper();

	float getSnap() { return m_snap; }
	void setSnap(float value) { m_snap = value; }
	float getLightOffset() { return m_lightOffset; }
	void setLightOffset(float value) { m_lightOffset = value; }
	Matrix3D& getDepthProjection();
	float getDepth() { return m_maxZ - m_minZ; }

protected:
	void updateDepthProjection(Camera3D* viewCamera) override;
	void drawDepthMap(TextureBase* target, Scene3D* scene, DepthRenderer* renderer) override;
	void updateCullPlanes(Camera3D* viewCamera);
	void updateProjectionFromFrustumCorners(Camera3D* viewCamera, float* corners, Matrix3D& matrix);

protected:
	Camera3D* m_overallDepthCamera;
	float m_localFrustum[24];
	float m_lightOffset;
	Matrix3D m_matrix;
	FreeMatrixLens m_overallDepthLens;
	float m_snap;
	PlaneVector m_cullPlanes;
	float m_minZ;
	float m_maxZ;
};

AWAY_NAMESPACE_END