#include "OrthographicLens.h"
#include "Vector3D.h"

USING_AWAY_NAMESPACE

OrthographicLens::OrthographicLens(float projectionHeight)
{
	m_projectionHeight = projectionHeight;
}

void OrthographicLens::setProjectionHeight(float value)
{
	if (value != m_projectionHeight)
	{
		m_projectionHeight = value;
		invalidateMatrix();
	}
}

Vector3D* OrthographicLens::unproject(float nX, float nY, float sZ, Vector3D* result)
{
	Vector3D translation;
	getMatrix().copyColumnTo(3, translation);
	result->m_x = nX + translation.m_x;
	result->m_y = -nY + translation.m_y;
	result->m_z = sZ;
	result->m_w = 1;

	Matrix3D::transformVector(getUnprojectionMatrix(), result, result);

	//z is unaffected by transform
	result->m_z = sZ;
	return result;
}

void OrthographicLens::updateMatrix()
{
	float* raw = m_matrix.m_rawData;
	float yMax = m_projectionHeight * .5f;
	float xMax = yMax * m_aspectRatio;
	float left, right, top, bottom;

	if (m_scissorRect.m_x == 0 && m_scissorRect.m_y == 0 && m_scissorRect.m_width == m_viewPort.m_width && m_scissorRect.m_height == m_viewPort.m_height)
	{
		// assume symmetric frustum
		left = -xMax;
		right = xMax;
		top = -yMax;
		bottom = yMax;

		raw[0] = 2 / (m_projectionHeight * m_aspectRatio);
		raw[5] = 2 / m_projectionHeight;
		raw[10] = 1 / (m_far - m_near);
		raw[14] = m_near / (m_near - m_far);
		raw[1] = raw[2] = raw[3] = raw[4] = raw[6] = raw[7] = raw[8] = raw[9] = raw[11] = raw[12] = raw[13] = 0;
		raw[15] = 1;
	}
	else
	{
		float xWidth = xMax * m_viewPort.m_width / m_scissorRect.m_width;
		float yHgt = yMax * m_viewPort.m_height / m_scissorRect.m_height;
		float center = xMax * (m_scissorRect.m_x * 2 - m_viewPort.m_width) / m_scissorRect.m_width + xMax;
		float middle = -yMax * (m_scissorRect.m_y * 2 - m_viewPort.m_height) / m_scissorRect.m_height - yMax;

		left = center - xWidth;
		right = center + xWidth;
		top = middle - yHgt;
		bottom = middle + yHgt;

		raw[0] = 2 / (right - left);
		raw[5] = -2 / (top - bottom);
		raw[10] = 1 / (m_far - m_near);

		raw[12] = (right + left) / (right - left);
		raw[13] = (bottom + top) / (bottom - top);
		raw[14] = m_near / (m_near - m_far);

		raw[1] = raw[2] = raw[3] = raw[4] = raw[6] = raw[7] = raw[8] = raw[9] = raw[11] = 0;
		raw[15] = 1;
	}

	m_frustumCorners[0] = m_frustumCorners[9] = m_frustumCorners[12] = m_frustumCorners[21] = left;
	m_frustumCorners[3] = m_frustumCorners[6] = m_frustumCorners[15] = m_frustumCorners[18] = right;
	m_frustumCorners[1] = m_frustumCorners[4] = m_frustumCorners[13] = m_frustumCorners[16] = top;
	m_frustumCorners[7] = m_frustumCorners[10] = m_frustumCorners[19] = m_frustumCorners[22] = bottom;
	m_frustumCorners[2] = m_frustumCorners[5] = m_frustumCorners[8] = m_frustumCorners[11] = m_near;
	m_frustumCorners[14] = m_frustumCorners[17] = m_frustumCorners[20] = m_frustumCorners[23] = m_far;
	m_matrixInvalid = false;
}