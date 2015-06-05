#pragma once

#include "Common.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * An abstract base class for all bounding volume classes. It should not be instantiated directly.
 */
class BoundingVolumeBase
{
public:
	BoundingVolumeBase();
	Vector3D* getMax() { return &m_max; }
	Vector3D* getMin() { return &m_min; }

	float* getAABBPoints();
	WireframePrimitiveBase* getBoundingRenderable();

	virtual void nullify(); // Sets the bounds to zero size.
	void disposeRenderable();

	virtual void fromGeometry(Geometry* geometry); // Updates the bounds to fit a Geometry object.
	void fromSkeletonPose(SkeletonPose* skeletonPose);
	virtual void fromSphere(Vector3D& center, float radius); // Sets the bound to fit a given sphere.
	virtual void fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ); // Sets the bounds to the given extrema.

	virtual bool isInFrustum(PlaneVector* planes, int numPlanes) = 0; // Tests if the bounds are in the camera frustum.
	bool overlaps(BoundingVolumeBase* bounds); // Tests if the bounds overlap other bounds, treating both bounds as AABBs.
	virtual float rayIntersection(Vector3D& position, Vector3D& direction, Vector3D& targetNormal); // Method for calculating whether an intersection of the given ray occurs with the bounding volume.
	virtual bool containsPoint(Vector3D& position); // Method for calculating whether the given position is contained within the bounding volume.

	void merge(BoundingVolumeBase* bounds);
	virtual void copyFrom(BoundingVolumeBase* bounds);
	virtual void transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix) = 0;

protected:
	void updateAABBPoints();
	virtual void updateBoundingRenderable() = 0;
	virtual WireframePrimitiveBase* createBoundingRenderable() = 0;

protected:
	Vector3D m_min;
	Vector3D m_max;
	float m_aabbPoints[24];
	bool m_aabbPointsDirty;
	WireframePrimitiveBase* m_boundingRenderable;
};

AWAY_NAMESPACE_END