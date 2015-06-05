#pragma once

#include "Common.h"
#include "LensBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * FreeMatrixLens provides a projection lens that exposes a full projection matrix, rather than provide one through
 * more user-friendly settings. Whenever the matrix is updated, it needs to be reset in order to trigger an update.
 */
class FreeMatrixLens : public LensBase
{
public:
	FreeMatrixLens();

	void setNear(float value) override;
	void setFar(float value) override;
	void setAspectRatio(float value) override;
	Vector3D* unproject(float nX, float nY, float sZ, Vector3D* result) override;

protected:
	void updateMatrix() override;
};

AWAY_NAMESPACE_END