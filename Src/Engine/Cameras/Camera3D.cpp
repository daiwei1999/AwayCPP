#include "Camera3D.h"
#include "CameraNode.h"
#include "NullBounds.h"
#include "PerspectiveLens.h"
#include "LensEvent.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

Camera3D::Camera3D(LensBase* lens)
{
	m_bounds = new NullBounds();
	m_worldBounds = new NullBounds();
	m_lens = lens ? lens : new PerspectiveLens();
	m_lens->addEventListener(LensEvent::MATRIX_CHANGED, CALLBACK_OBJECT(onLensMatrixChanged));

	//setup default frustum planes
	m_frustumPlanes.resize(6);

	m_viewProjectionDirty = true;
	m_frustumPlanesDirty = true;
	setZ(-1000);
}

void Camera3D::setLens(LensBase* value)
{
	if (value == m_lens)
		return;

	assert(value); // Lens cannot be NULL!

	m_lens->removeEventListener(LensEvent::MATRIX_CHANGED, CALLBACK_OBJECT(onLensMatrixChanged));
	m_lens = value;
	m_lens->addEventListener(LensEvent::MATRIX_CHANGED, CALLBACK_OBJECT(onLensMatrixChanged));
}

Matrix3D& Camera3D::getViewProjection()
{
	if (m_viewProjectionDirty)
	{
		m_viewProjection.copyFrom(getInverseSceneTransform());
		m_viewProjection.append(m_lens->getMatrix());
		m_viewProjectionDirty = false;
	}
	return m_viewProjection;
}

PlaneVector* Camera3D::getFrustumPlanes()
{
	if (m_frustumPlanesDirty)
		updateFrustum();

	return &m_frustumPlanes;
}

void Camera3D::project(Vector3D* point3d, Vector3D* result)
{
	m_lens->project(Matrix3D::transformVector(getInverseSceneTransform(), point3d, result), result);
}

void Camera3D::unproject(float nX, float nY, float sZ, Vector3D* result)
{
	Matrix3D::transformVector(getSceneTransform(), m_lens->unproject(nX, nY, sZ, result), result);
}

void Camera3D::getRay(float nX, float nY, float sZ, Vector3D* result)
{
	Matrix3D::deltaTransformVector(getSceneTransform(), m_lens->unproject(nX, nY, sZ, result), result);
}

void Camera3D::updateFrustum()
{
	Matrix3D& matrix = getViewProjection();
	float c11 = matrix.m_rawData[0];
	float c12 = matrix.m_rawData[4];
	float c13 = matrix.m_rawData[8];
	float c14 = matrix.m_rawData[12];
	float c21 = matrix.m_rawData[1];
	float c22 = matrix.m_rawData[5];
	float c23 = matrix.m_rawData[9];
	float c24 = matrix.m_rawData[13];
	float c31 = matrix.m_rawData[2];
	float c32 = matrix.m_rawData[6];
	float c33 = matrix.m_rawData[10];
	float c34 = matrix.m_rawData[14];
	float c41 = matrix.m_rawData[3];
	float c42 = matrix.m_rawData[7];
	float c43 = matrix.m_rawData[11];
	float c44 = matrix.m_rawData[15];

	// left plane
	Plane3D& p0 = m_frustumPlanes[0];
	float a = c41 + c11;
	float b = c42 + c12;
	float c = c43 + c13;
	float invLen = 1 / std::sqrt(a * a + b * b + c * c);
	p0.m_a = a * invLen;
	p0.m_b = b * invLen;
	p0.m_c = c * invLen;
	p0.m_d = -(c44 + c14) * invLen;

	// right plane
	Plane3D& p1 = m_frustumPlanes[1];
	a = c41 - c11;
	b = c42 - c12;
	c = c43 - c13;
	invLen = 1 / std::sqrt(a * a + b * b + c * c);
	p1.m_a = a * invLen;
	p1.m_b = b * invLen;
	p1.m_c = c * invLen;
	p1.m_d = (c14 - c44) * invLen;

	// bottom
	Plane3D& p2 = m_frustumPlanes[2];
	a = c41 + c21;
	b = c42 + c22;
	c = c43 + c23;
	invLen = 1 / std::sqrt(a * a + b * b + c * c);
	p2.m_a = a * invLen;
	p2.m_b = b * invLen;
	p2.m_c = c * invLen;
	p2.m_d = -(c44 + c24) * invLen;

	// top
	Plane3D& p3 = m_frustumPlanes[3];
	a = c41 - c21;
	b = c42 - c22;
	c = c43 - c23;
	invLen = 1 / std::sqrt(a * a + b * b + c * c);
	p3.m_a = a * invLen;
	p3.m_b = b * invLen;
	p3.m_c = c * invLen;
	p3.m_d = (c24 - c44) * invLen;

	// near
	Plane3D& p4 = m_frustumPlanes[4];
	invLen = 1 / std::sqrt(c31 * c31 + c32 * c32 + c33 * c33);
	p4.m_a = c31 * invLen;
	p4.m_b = c32 * invLen;
	p4.m_c = c33 * invLen;
	p4.m_d = -c34 * invLen;

	// far
	Plane3D& p5 = m_frustumPlanes[5];
	a = c41 - c31;
	b = c42 - c32;
	c = c43 - c33;
	invLen = 1 / std::sqrt(a * a + b * b + c * c);
	p5.m_a = a * invLen;
	p5.m_b = b * invLen;
	p5.m_c = c * invLen;
	p5.m_d = (c34 - c44) * invLen;

	m_frustumPlanesDirty = false;
}

void Camera3D::onLensMatrixChanged(Event* event)
{
	m_viewProjectionDirty = true;
	m_frustumPlanesDirty = true;
}

EntityNode* Camera3D::createEntityPartitionNode()
{
	return new CameraNode(this);
}

void Camera3D::updateBounds()
{
	m_bounds->nullify();
	m_boundsInvalid = false;
}

void Camera3D::invalidateSceneTransform()
{
	Entity::invalidateSceneTransform();
	m_viewProjectionDirty = true;
	m_frustumPlanesDirty = true;
}