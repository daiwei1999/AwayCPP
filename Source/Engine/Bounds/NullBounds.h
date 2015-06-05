#pragma once

#include "Common.h"
#include "BoundingVolumeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * NullBounds represents a debug bounding "volume" that is either considered always in, or always out of the frustum.
 * NullBounds is useful for entities that are always considered in the frustum, such as directional lights or skyboxes.
 */
class NullBounds : public BoundingVolumeBase
{
public:
	NullBounds(bool alwaysIn = true, WireframePrimitiveBase* renderable = nullptr);

	void fromGeometry(Geometry* geometry) override;
	void fromSphere(Vector3D& center, float radius) override;
	void fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) override;

	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	void copyFrom(BoundingVolumeBase* bounds) override;
	void transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix) override;

protected:
	void updateBoundingRenderable() override;
	WireframePrimitiveBase* createBoundingRenderable() override;

private:
	bool m_alwaysIn;
	WireframePrimitiveBase* m_renderable;
};

AWAY_NAMESPACE_END