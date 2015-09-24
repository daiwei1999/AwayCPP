#include "PerspectiveLens.h"
#include "MathConsts.h"
#include "Vector3D.h"

USING_AWAY_NAMESPACE

PerspectiveLens::PerspectiveLens(float fieldOfView)
{
	setFieldOfView(fieldOfView);
}

void PerspectiveLens::setFieldOfView(float value)
{
	if (value == m_fieldOfView)
		return;

	m_fieldOfView = value;
	m_focalLengthInv = std::tan(m_fieldOfView * MathConsts::DEGREES_TO_RADIANS * .5f);
	m_focalLength = 1 / m_focalLengthInv;
	invalidateMatrix();
}

void PerspectiveLens::setFocalLength(float value)
{
	if (value == m_focalLength)
		return;

	m_focalLength = value;
	m_focalLengthInv = 1 / m_focalLength;
	m_fieldOfView = std::atan(m_focalLengthInv) * MathConsts::RADIANS_TO_DEGREES * 2;
	invalidateMatrix();
}

Vector3D* PerspectiveLens::unproject(float nX, float nY, float sZ, Vector3D* result)
{
	result->m_x = nX * sZ;
	result->m_y = -nY * sZ;
	result->m_z = sZ;
	result->m_w = 1;

	Matrix3D::transformVector(getUnprojectionMatrix(), result, result);

	//z is unaffected by transform
	result->m_z = sZ;
	return result;
}

void PerspectiveLens::updateMatrix()
{
	float yMax = m_near * m_focalLengthInv;
	float xMax = yMax * m_aspectRatio;

	float left, right, top, bottom;
	float(&raw)[16] = m_matrix.m_rawData;
	if (m_scissorRect.m_x == 0 && m_scissorRect.m_y == 0 && m_scissorRect.m_width == m_viewPort.m_width && m_scissorRect.m_height == m_viewPort.m_height)
	{
		// assume unscissored frustum
		left = -xMax;
		right = xMax;
		top = -yMax;
		bottom = yMax;

		raw[0] = m_near / xMax;
		raw[5] = m_near / yMax;
		raw[10] = m_far / (m_far - m_near);
		raw[11] = 1;
		raw[1] = raw[2] = raw[3] = raw[4] =	raw[6] = raw[7] = raw[8] = raw[9] = raw[12] = raw[13] = raw[15] = 0;
		raw[14] = -m_near * raw[10];
	}
	else
	{
		// assume scissored frustum
		float xWidth = xMax * m_viewPort.m_width / m_scissorRect.m_width;
		float yHgt = yMax * m_viewPort.m_height / m_scissorRect.m_height;
		float center = xMax * (m_scissorRect.m_x * 2 - m_viewPort.m_width) / m_scissorRect.m_width + xMax;
		float middle = -yMax * (m_scissorRect.m_y * 2 - m_viewPort.m_height) / m_scissorRect.m_height - yMax;

		left = center - xWidth;
		right = center + xWidth;
		top = middle - yHgt;
		bottom = middle + yHgt;

		raw[0] = 2 * m_near / (right - left);
		raw[5] = 2 * m_near / (bottom - top);
		raw[8] = (right + left) / (right - left);
		raw[9] = (bottom + top) / (bottom - top);
		raw[10] = (m_far + m_near) / (m_far - m_near);
		raw[11] = 1;
		raw[1] = raw[2] = raw[3] = raw[4] = raw[6] = raw[7] = raw[12] = raw[13] = raw[15] = 0;
		raw[14] = -2 * m_far * m_near / (m_far - m_near);
	}

	float yMaxFar = m_far * m_focalLengthInv;
	float xMaxFar = yMaxFar * m_aspectRatio;

	m_frustumCorners[0] = m_frustumCorners[9] = left;
	m_frustumCorners[3] = m_frustumCorners[6] = right;
	m_frustumCorners[1] = m_frustumCorners[4] = top;
	m_frustumCorners[7] = m_frustumCorners[10] = bottom;

	m_frustumCorners[12] = m_frustumCorners[21] = -xMaxFar;
	m_frustumCorners[15] = m_frustumCorners[18] = xMaxFar;
	m_frustumCorners[13] = m_frustumCorners[16] = -yMaxFar;
	m_frustumCorners[19] = m_frustumCorners[22] = yMaxFar;

	m_frustumCorners[2] = m_frustumCorners[5] = m_frustumCorners[8] = m_frustumCorners[11] = m_near;
	m_frustumCorners[14] = m_frustumCorners[17] = m_frustumCorners[20] = m_frustumCorners[23] = m_far;

	m_matrixInvalid = false;
}