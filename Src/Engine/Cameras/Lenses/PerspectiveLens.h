#pragma once

#include "Common.h"
#include "LensBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The PerspectiveLens object provides a projection matrix that projects 3D geometry with perspective distortion.
 */
class PerspectiveLens : public LensBase
{
public:
	PerspectiveLens(float fieldOfView = 60);

	float getFieldOfView() { return m_fieldOfView; }
	void setFieldOfView(float value);

	float getFocalLength() { return m_focalLength; }
	void setFocalLength(float value);

	Vector3D* unproject(float nX, float nY, float sZ, Vector3D* result) override;

protected:
	void updateMatrix() override;

private:
	float m_fieldOfView;
	float m_focalLength;
	float m_focalLengthInv;
};

AWAY_NAMESPACE_END