#pragma once

#include "Common.h"
#include "BoundingVolumeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * AxisAlignedBoundingBox represents a bounding box volume that has its planes aligned to the local coordinate axes of the bounded object.
 * This is useful for most meshes.
 */
class AxisAlignedBoundingBox : public BoundingVolumeBase
{
public:
	AxisAlignedBoundingBox();

	float getCenterX() { return m_centerX; }
	float getCenterY() { return m_centerY; }
	float getCenterZ() { return m_centerZ; }
	float getHalfExtentsX() { return m_halfExtentsX; }
	float getHalfExtentsY() { return m_halfExtentsY; }
	float getHalfExtentsZ() { return m_halfExtentsZ; }

	void nullify() override;
	bool isInFrustum(PlaneVector* planes, int numPlanes) override;
	float rayIntersection(Vector3D& position, Vector3D& direction, Vector3D& targetNormal) override;
	bool containsPoint(Vector3D& position) override;
	void fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) override;

	void copyFrom(BoundingVolumeBase* bounds) override;
	void transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix) override;

protected:
	void updateBoundingRenderable() override;
	WireframePrimitiveBase* createBoundingRenderable() override;

private:
	float m_centerX;
	float m_centerY;
	float m_centerZ;
	float m_halfExtentsX;
	float m_halfExtentsY;
	float m_halfExtentsZ;
};

AWAY_NAMESPACE_END