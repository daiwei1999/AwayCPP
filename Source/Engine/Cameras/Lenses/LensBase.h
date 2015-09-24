#pragma once

#include "Common.h"
#include "EventDispatcher.h"
#include "Matrix3D.h"
#include "Rectangle.h"

AWAY_NAMESPACE_BEGIN

/**
 * An abstract base class for all lens classes. Lens objects provides a projection matrix that transforms 3D geometry to normalized homogeneous coordinates.
 */
class LensBase : public EventDispatcher
{
public:
	LensBase();

	Matrix3D& getMatrix();
	void setMatrix(Matrix3D& value);

	float getNear() { return m_near; }
	virtual void setNear(float value);

	float getFar() { return m_far; }
	virtual void setFar(float value);

	float getAspectRatio() { return m_aspectRatio; }
	virtual void setAspectRatio(float value);

	float* getFrustumCorners() { return m_frustumCorners; }
	Matrix3D& getUnprojectionMatrix();

	Vector3D* project(Vector3D* point3d, Vector3D* result);
	virtual Vector3D* unproject(float nX, float nY, float sZ, Vector3D* result) = 0;

	void updateScissorRect(int x, int y, int width, int height);
	void updateViewport(int x, int y, int width, int height);

protected:
	void invalidateMatrix();
	virtual void updateMatrix() = 0;

protected:
	Matrix3D m_matrix;
	Rectangle<int> m_scissorRect;
	Rectangle<int> m_viewPort;
	float m_near;
	float m_far;
	float m_aspectRatio;
	bool m_matrixInvalid;
	float m_frustumCorners[24];

private:
	Matrix3D m_unprojection;
	bool m_unprojectionInvalid;
};

AWAY_NAMESPACE_END