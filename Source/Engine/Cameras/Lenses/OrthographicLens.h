#pragma once

#include "Common.h"
#include "LensBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The PerspectiveLens object provides a projection matrix that projects 3D geometry isometrically. This entails
 * there is no perspective distortion, and lines that are parallel in the scene will remain parallel on the screen.
 */
class OrthographicLens : public LensBase
{
public:
	OrthographicLens(float projectionHeight = 500);

	float getProjectionHeight() { return m_projectionHeight; }
	void setProjectionHeight(float value);

	Vector3D* unproject(float nX, float nY, float sZ, Vector3D* result) override;

protected:
	void updateMatrix() override;

private:
	float m_projectionHeight;
};

AWAY_NAMESPACE_END