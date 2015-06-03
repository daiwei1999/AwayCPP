#pragma once

#include "Common.h"
#include "BoundingVolumeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * BoundingSphere represents a spherical bounding volume defined by a center point and a radius.
 * This bounding volume is useful for point lights.
 */
class BoundingSphere : public BoundingVolumeBase
{
public:
	BoundingSphere();

	void nullify() override;
	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	void fromSphere(Vector3D& center, float radius) override;
	void fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) override;
	float rayIntersection(Vector3D& position, Vector3D& direction, Vector3D& targetNormal) override;
	bool containsPoint(Vector3D& position) override;
	void copyFrom(BoundingVolumeBase* bounds) override;
	void transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix) override;

protected:
	void updateBoundingRenderable() override;
	WireframePrimitiveBase* createBoundingRenderable() override;

private:
	float m_radius;
	float m_centerX;
	float m_centerY;
	float m_centerZ;
};

AWAY_NAMESPACE_END