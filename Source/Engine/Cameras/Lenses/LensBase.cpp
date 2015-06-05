#include "LensBase.h"
#include "Vector3D.h"
#include "LensEvent.h"

USING_AWAY_NAMESPACE

LensBase::LensBase()
{
	m_near = 20;
	m_far = 3000;
	m_aspectRatio = 1;
	m_matrixInvalid = true;
	m_unprojectionInvalid = true;
}

Matrix3D& LensBase::getMatrix()
{
	if (m_matrixInvalid)
	{
		updateMatrix();
		m_matrixInvalid = false;
	}
	return m_matrix;
}

void LensBase::setMatrix(Matrix3D& value)
{
	m_matrix.copyFrom(value);
	invalidateMatrix();
}

void LensBase::setNear(float value)
{
	if (value == m_near)
		return;

	m_near = value;
	invalidateMatrix();
}

void LensBase::setFar(float value)
{
	if (value == m_far)
		return;

	m_far = value;
	invalidateMatrix();
}

void LensBase::setAspectRatio(float value)
{
	if (m_aspectRatio == value || (value * 0) != 0)
		return;

	m_aspectRatio = value;
	invalidateMatrix();
}

Matrix3D& LensBase::getUnprojectionMatrix()
{
	if (m_unprojectionInvalid)
	{
		m_unprojection.copyFrom(getMatrix());
		m_unprojection.invert();
		m_unprojectionInvalid = false;
	}
	return m_unprojection;
}

Vector3D* LensBase::project(Vector3D* point3d, Vector3D* result)
{
	float z = point3d->m_z;
	Matrix3D::transformVector(getMatrix(), point3d, result);
	result->m_x = result->m_x / result->m_w;
	result->m_y = -result->m_y / result->m_w;

	//z is unaffected by transform
	result->m_z = z;

	return result;
}

void LensBase::updateScissorRect(float x, float y, float width, float height)
{
	m_scissorRect.m_x = x;
	m_scissorRect.m_y = y;
	m_scissorRect.m_width = width;
	m_scissorRect.m_height = height;
	invalidateMatrix();
}

void LensBase::updateViewport(float x, float y, float width, float height)
{
	m_viewPort.m_x = x;
	m_viewPort.m_y = y;
	m_viewPort.m_width = width;
	m_viewPort.m_height = height;
	invalidateMatrix();
}

void LensBase::invalidateMatrix()
{
	m_matrixInvalid = true;
	m_unprojectionInvalid = true;

	// notify the camera that the lens matrix is changing. this will mark the 
	// viewProjectionMatrix in the camera as invalid, and force the matrix to
	// be re-queried from the lens, and therefore rebuilt.
	LensEvent event(LensEvent::MATRIX_CHANGED, this);
	dispatchEvent(&event);
}