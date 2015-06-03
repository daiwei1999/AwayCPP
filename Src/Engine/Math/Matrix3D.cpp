#include "Matrix3D.h"
#include "Vector3D.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

Matrix3D::Matrix3D(float m00, float m10, float m20, float m30,
	float m01, float m11, float m21, float m31,
	float m02, float m12, float m22, float m32,
	float m03, float m13, float m23, float m33)
{
	init(m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33);
}

Matrix3D::Matrix3D(const Matrix3D& matrix)
{
	const float(&raw)[16] = matrix.m_rawData;
	init(raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7], raw[8], raw[9], raw[10], raw[11], raw[12], raw[13], raw[14], raw[15]);
}

void Matrix3D::append(const Matrix3D& lhs)
{
	float m111 = m_rawData[0], m121 = m_rawData[4], m131 = m_rawData[8], m141 = m_rawData[12];
	float m112 = m_rawData[1], m122 = m_rawData[5], m132 = m_rawData[9], m142 = m_rawData[13];
	float m113 = m_rawData[2], m123 = m_rawData[6], m133 = m_rawData[10], m143 = m_rawData[14];
	float m114 = m_rawData[3], m124 = m_rawData[7], m134 = m_rawData[11], m144 = m_rawData[15];
	float m211 = lhs.m_rawData[0], m221 = lhs.m_rawData[4], m231 = lhs.m_rawData[8], m241 = lhs.m_rawData[12];
	float m212 = lhs.m_rawData[1], m222 = lhs.m_rawData[5], m232 = lhs.m_rawData[9], m242 = lhs.m_rawData[13];
	float m213 = lhs.m_rawData[2], m223 = lhs.m_rawData[6], m233 = lhs.m_rawData[10], m243 = lhs.m_rawData[14];
	float m214 = lhs.m_rawData[3], m224 = lhs.m_rawData[7], m234 = lhs.m_rawData[11], m244 = lhs.m_rawData[15];

	init(m111 * m211 + m112 * m221 + m113 * m231 + m114 * m241,
		 m111 * m212 + m112 * m222 + m113 * m232 + m114 * m242,
		 m111 * m213 + m112 * m223 + m113 * m233 + m114 * m243,
		 m111 * m214 + m112 * m224 + m113 * m234 + m114 * m244,
		 m121 * m211 + m122 * m221 + m123 * m231 + m124 * m241,
		 m121 * m212 + m122 * m222 + m123 * m232 + m124 * m242,
		 m121 * m213 + m122 * m223 + m123 * m233 + m124 * m243,
		 m121 * m214 + m122 * m224 + m123 * m234 + m124 * m244,
		 m131 * m211 + m132 * m221 + m133 * m231 + m134 * m241,
		 m131 * m212 + m132 * m222 + m133 * m232 + m134 * m242,
		 m131 * m213 + m132 * m223 + m133 * m233 + m134 * m243,
		 m131 * m214 + m132 * m224 + m133 * m234 + m134 * m244,
		 m141 * m211 + m142 * m221 + m143 * m231 + m144 * m241,
		 m141 * m212 + m142 * m222 + m143 * m232 + m144 * m242,
		 m141 * m213 + m142 * m223 + m143 * m233 + m144 * m243,
		 m141 * m214 + m142 * m224 + m143 * m234 + m144 * m244);
}

void Matrix3D::appendRotation(float degrees, const Vector3D& axis)
{
	Matrix3D rot;
	getAxisRotation(axis.m_x, axis.m_y, axis.m_z, degrees, rot);
	append(rot);
}

void Matrix3D::appendScale(float xScale, float yScale, float zScale)
{
	m_rawData[0] *= xScale;
	m_rawData[4] *= xScale;
	m_rawData[8] *= xScale;
	m_rawData[12] *= xScale;
	m_rawData[1] *= yScale;
	m_rawData[5] *= yScale;
	m_rawData[9] *= yScale;
	m_rawData[13] *= yScale;
	m_rawData[2] *= zScale;
	m_rawData[6] *= zScale;
	m_rawData[10] *= zScale;
	m_rawData[14] *= zScale;
}

void Matrix3D::appendTranslation(float x, float y, float z)
{
	m_rawData[12] += x;
	m_rawData[13] += y;
	m_rawData[14] += z;
}

void Matrix3D::prepend(const Matrix3D& rhs)
{
	float m111 = rhs.m_rawData[0], m121 = rhs.m_rawData[4], m131 = rhs.m_rawData[8], m141 = rhs.m_rawData[12];
	float m112 = rhs.m_rawData[1], m122 = rhs.m_rawData[5], m132 = rhs.m_rawData[9], m142 = rhs.m_rawData[13];
	float m113 = rhs.m_rawData[2], m123 = rhs.m_rawData[6], m133 = rhs.m_rawData[10], m143 = rhs.m_rawData[14];
	float m114 = rhs.m_rawData[3], m124 = rhs.m_rawData[7], m134 = rhs.m_rawData[11], m144 = rhs.m_rawData[15];
	float m211 = m_rawData[0], m221 = m_rawData[4], m231 = m_rawData[8], m241 = m_rawData[12];
	float m212 = m_rawData[1], m222 = m_rawData[5], m232 = m_rawData[9], m242 = m_rawData[13];
	float m213 = m_rawData[2], m223 = m_rawData[6], m233 = m_rawData[10], m243 = m_rawData[14];
	float m214 = m_rawData[3], m224 = m_rawData[7], m234 = m_rawData[11], m244 = m_rawData[15];

	init(m111 * m211 + m112 * m221 + m113 * m231 + m114 * m241,
		 m111 * m212 + m112 * m222 + m113 * m232 + m114 * m242,
		 m111 * m213 + m112 * m223 + m113 * m233 + m114 * m243,
		 m111 * m214 + m112 * m224 + m113 * m234 + m114 * m244,
		 m121 * m211 + m122 * m221 + m123 * m231 + m124 * m241,
		 m121 * m212 + m122 * m222 + m123 * m232 + m124 * m242,
		 m121 * m213 + m122 * m223 + m123 * m233 + m124 * m243,
		 m121 * m214 + m122 * m224 + m123 * m234 + m124 * m244,
		 m131 * m211 + m132 * m221 + m133 * m231 + m134 * m241,
		 m131 * m212 + m132 * m222 + m133 * m232 + m134 * m242,
		 m131 * m213 + m132 * m223 + m133 * m233 + m134 * m243,
		 m131 * m214 + m132 * m224 + m133 * m234 + m134 * m244,
		 m141 * m211 + m142 * m221 + m143 * m231 + m144 * m241,
		 m141 * m212 + m142 * m222 + m143 * m232 + m144 * m242,
		 m141 * m213 + m142 * m223 + m143 * m233 + m144 * m243,
		 m141 * m214 + m142 * m224 + m143 * m234 + m144 * m244);
}

void Matrix3D::prependRotation(float degrees, const Vector3D& axis)
{
	Matrix3D rot;
	getAxisRotation(axis.m_x, axis.m_y, axis.m_z, degrees, rot);
	prepend(rot);
}

void Matrix3D::prependScale(float xScale, float yScale, float zScale)
{
	m_rawData[0] *= xScale;
	m_rawData[1] *= xScale;
	m_rawData[2] *= xScale;
	m_rawData[3] *= xScale;
	m_rawData[4] *= yScale;
	m_rawData[5] *= yScale;
	m_rawData[6] *= yScale;
	m_rawData[7] *= yScale;
	m_rawData[8] *= zScale;
	m_rawData[9] *= zScale;
	m_rawData[10] *= zScale;
	m_rawData[11] *= zScale;
}

void Matrix3D::prependTranslation(float x, float y, float z)
{
	m_rawData[12] = m_rawData[0] * x + m_rawData[4] * y + m_rawData[8] * z + m_rawData[12];
	m_rawData[13] = m_rawData[1] * x + m_rawData[5] * y + m_rawData[9] * z + m_rawData[13];
	m_rawData[14] = m_rawData[2] * x + m_rawData[6] * y + m_rawData[10] * z + m_rawData[14];
	m_rawData[15] = m_rawData[3] * x + m_rawData[7] * y + m_rawData[11] * z + m_rawData[15];
}

void Matrix3D::copyColumnFrom(int column, const Vector3D& vector3D)
{
	switch (column)
	{
	case 0:
		m_rawData[0] = vector3D.m_x;
		m_rawData[1] = vector3D.m_y;
		m_rawData[2] = vector3D.m_z;
		m_rawData[3] = vector3D.m_w;
		break;
	case 1:
		m_rawData[4] = vector3D.m_x;
		m_rawData[5] = vector3D.m_y;
		m_rawData[6] = vector3D.m_z;
		m_rawData[7] = vector3D.m_w;
		break;
	case 2:
		m_rawData[8] = vector3D.m_x;
		m_rawData[9] = vector3D.m_y;
		m_rawData[10] = vector3D.m_z;
		m_rawData[11] = vector3D.m_w;
		break;
	case 3:
		m_rawData[12] = vector3D.m_x;
		m_rawData[13] = vector3D.m_y;
		m_rawData[14] = vector3D.m_z;
		m_rawData[15] = vector3D.m_w;
		break;
	}
}

void Matrix3D::copyColumnTo(int column, Vector3D& vector3D) const
{
	switch (column)
	{
	case 0:
		vector3D.m_x = m_rawData[0];
		vector3D.m_y = m_rawData[1];
		vector3D.m_z = m_rawData[2];
		vector3D.m_w = m_rawData[3];
		break;
	case 1:
		vector3D.m_x = m_rawData[4];
		vector3D.m_y = m_rawData[5];
		vector3D.m_z = m_rawData[6];
		vector3D.m_w = m_rawData[7];
		break;
	case 2:
		vector3D.m_x = m_rawData[8];
		vector3D.m_y = m_rawData[9];
		vector3D.m_z = m_rawData[10];
		vector3D.m_w = m_rawData[11];
		break;
	case 3:
		vector3D.m_x = m_rawData[12];
		vector3D.m_y = m_rawData[13];
		vector3D.m_z = m_rawData[14];
		vector3D.m_w = m_rawData[15];
		break;
	}
}

void Matrix3D::copyFrom(const Matrix3D& sourceMatrix3D)
{
	for (int i = 0; i < 16; i++)
		m_rawData[i] = sourceMatrix3D.m_rawData[i];
}

void Matrix3D::copyRawDataFrom(const float* data, int index, bool transpose)
{
	if (transpose)
		this->transpose();

	for (int i = 0; i < 16; i++)
		m_rawData[i] = data[i + index];

	if (transpose)
		this->transpose();
}

void Matrix3D::copyRawDataTo(float* data, int index, bool transpose) const
{
	if (transpose)
	{
		int offset = index;
		for (int i = 0; i < 4; i++)
		{
			data[offset] = m_rawData[i];
			data[offset + 1] = m_rawData[i + 4];
			data[offset + 2] = m_rawData[i + 8];
			data[offset + 3] = m_rawData[i + 12];
			offset += 4;
		}
	}
	else
	{
		for (int i = 0; i < 16; i++)
			data[i + index] = m_rawData[i];
	}
}

void Matrix3D::copyRowFrom(int row, const Vector3D& vector3D)
{
	switch (row)
	{
	case 0:
		m_rawData[0] = vector3D.m_x;
		m_rawData[4] = vector3D.m_y;
		m_rawData[8] = vector3D.m_z;
		m_rawData[12] = vector3D.m_w;
		break;
	case 1:
		m_rawData[1] = vector3D.m_x;
		m_rawData[5] = vector3D.m_y;
		m_rawData[9] = vector3D.m_z;
		m_rawData[13] = vector3D.m_w;
		break;
	case 2:
		m_rawData[2] = vector3D.m_x;
		m_rawData[6] = vector3D.m_y;
		m_rawData[10] = vector3D.m_z;
		m_rawData[14] = vector3D.m_w;
		break;
	case 3:
		m_rawData[3] = vector3D.m_x;
		m_rawData[7] = vector3D.m_y;
		m_rawData[11] = vector3D.m_z;
		m_rawData[15] = vector3D.m_w;
		break;
	}
}

void Matrix3D::copyRowTo(int row, Vector3D& vector3D) const
{
	switch (row)
	{
	case 0:
		vector3D.m_x = m_rawData[0];
		vector3D.m_y = m_rawData[4];
		vector3D.m_z = m_rawData[8];
		vector3D.m_w = m_rawData[12];
		break;
	case 1:
		vector3D.m_x = m_rawData[1];
		vector3D.m_y = m_rawData[5];
		vector3D.m_z = m_rawData[9];
		vector3D.m_w = m_rawData[13];
		break;
	case 2:
		vector3D.m_x = m_rawData[2];
		vector3D.m_y = m_rawData[6];
		vector3D.m_z = m_rawData[10];
		vector3D.m_w = m_rawData[14];
		break;
	case 3:
		vector3D.m_x = m_rawData[3];
		vector3D.m_y = m_rawData[7];
		vector3D.m_z = m_rawData[11];
		vector3D.m_w = m_rawData[15];
		break;
	}
}

void Matrix3D::copyToMatrix3D(Matrix3D& dest) const
{
	for (int i = 0; i < 16; i++)
		dest.m_rawData[i] = m_rawData[i];
}

bool Matrix3D::invert()
{
	float s0 = m_rawData[0] * m_rawData[5] - m_rawData[1] * m_rawData[4];
	float s1 = m_rawData[0] * m_rawData[6] - m_rawData[2] * m_rawData[4];
	float s2 = m_rawData[0] * m_rawData[7] - m_rawData[3] * m_rawData[4];
	float s3 = m_rawData[1] * m_rawData[6] - m_rawData[2] * m_rawData[5];
	float s4 = m_rawData[1] * m_rawData[7] - m_rawData[3] * m_rawData[5];
	float s5 = m_rawData[2] * m_rawData[7] - m_rawData[3] * m_rawData[6];

	float c0 = m_rawData[8] * m_rawData[13] - m_rawData[9] * m_rawData[12];
	float c1 = m_rawData[8] * m_rawData[14] - m_rawData[10] * m_rawData[12];
	float c2 = m_rawData[8] * m_rawData[15] - m_rawData[11] * m_rawData[12];
	float c3 = m_rawData[9] * m_rawData[14] - m_rawData[10] * m_rawData[13];
	float c4 = m_rawData[9] * m_rawData[15] - m_rawData[11] * m_rawData[13];
	float c5 = m_rawData[10] * m_rawData[15] - m_rawData[11] * m_rawData[14];

	float det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
	if (std::abs(det) < 0.00000000001f)
		return false;

	det = 1 / det;
	init((m_rawData[5] * c5 - m_rawData[6] * c4 + m_rawData[7] * c3) * det,
		 (-m_rawData[1] * c5 + m_rawData[2] * c4 - m_rawData[3] * c3) * det,
		 (m_rawData[13] * s5 - m_rawData[14] * s4 + m_rawData[15] * s3) * det,
		 (-m_rawData[9] * s5 + m_rawData[10] * s4 - m_rawData[11] * s3) * det,
		 (-m_rawData[4] * c5 + m_rawData[6] * c2 - m_rawData[7] * c1) * det,
		 (m_rawData[0] * c5 - m_rawData[2] * c2 + m_rawData[3] * c1) * det,
		 (-m_rawData[12] * s5 + m_rawData[14] * s2 - m_rawData[15] * s1) * det,
		 (m_rawData[8] * s5 - m_rawData[10] * s2 + m_rawData[11] * s1) * det,
		 (m_rawData[4] * c4 - m_rawData[5] * c2 + m_rawData[7] * c0) * det,
		 (-m_rawData[0] * c4 + m_rawData[1] * c2 - m_rawData[3] * c0) * det,
		 (m_rawData[12] * s4 - m_rawData[13] * s2 + m_rawData[15] * s0) * det,
		 (-m_rawData[8] * s4 + m_rawData[9] * s2 - m_rawData[11] * s0) * det,
		 (-m_rawData[4] * c3 + m_rawData[5] * c1 - m_rawData[6] * c0) * det,
		 (m_rawData[0] * c3 - m_rawData[1] * c1 + m_rawData[2] * c0) * det,
		 (-m_rawData[12] * s3 + m_rawData[13] * s1 - m_rawData[14] * s0) * det,
		 (m_rawData[8] * s3 - m_rawData[9] * s1 + m_rawData[10] * s0) * det);

	return true;
}

void Matrix3D::identity()
{
	m_rawData[0] = m_rawData[5] = m_rawData[10] = m_rawData[15] = 1;
	m_rawData[1] = m_rawData[2] = m_rawData[3] = m_rawData[4] = m_rawData[6] = m_rawData[7] = m_rawData[8] = m_rawData[9] = m_rawData[11] = m_rawData[12] = m_rawData[13] = m_rawData[14] = 0;
}

void Matrix3D::transpose()
{
	float temp = m_rawData[1];
	m_rawData[1] = m_rawData[4];
	m_rawData[4] = temp;
	temp = m_rawData[2];
	m_rawData[2] = m_rawData[8];
	m_rawData[8] = temp;
	temp = m_rawData[3];
	m_rawData[3] = m_rawData[12];
	m_rawData[12] = temp;
	temp = m_rawData[6];
	m_rawData[6] = m_rawData[9];
	m_rawData[9] = temp;
	temp = m_rawData[7];
	m_rawData[7] = m_rawData[13];
	m_rawData[13] = temp;
	temp = m_rawData[11];
	m_rawData[11] = m_rawData[14];
	m_rawData[14] = temp;
}

bool Matrix3D::recompose(const Vector3D* components, Orientation3D orientationStyle)
{
	float scaleX = components[2].m_x, scaleY = components[2].m_y, scaleZ = components[2].m_z;
	if (scaleX == 0 || scaleY == 0 || scaleZ == 0)
		return false;

	identity();

	if (orientationStyle == Orientation3D::EULER_ANGLES)
	{
		float cx = std::cos(components[1].m_x);
		float cy = std::cos(components[1].m_y);
		float cz = std::cos(components[1].m_z);
		float sx = std::sin(components[1].m_x);
		float sy = std::sin(components[1].m_y);
		float sz = std::sin(components[1].m_z);
		m_rawData[0] = cy * cz * scaleX;
		m_rawData[1] = cy * sz * scaleX;
		m_rawData[2] = -sy * scaleX;
		m_rawData[4] = (sx * sy * cz - cx * sz) * scaleY;
		m_rawData[5] = (sx * sy * sz + cx * cz) * scaleY;
		m_rawData[6] = sx * cy * scaleY;
		m_rawData[8] = (cx * sy * cz + sx * sz) * scaleZ;
		m_rawData[9] = (cx * sy * sz - sx * cz) * scaleZ;
		m_rawData[10] = cx * cy * scaleZ;
	}
	else
	{
		float x = components[1].m_x;
		float y = components[1].m_y;
		float z = components[1].m_z;
		float w = components[1].m_w;
		if (orientationStyle == Orientation3D::AXIS_ANGLE)
		{
			float hw = w * .5f;
			float shw = std::sin(hw);
			x *= shw;
			y *= shw;
			z *= shw;
			w = std::cos(hw);
		}
		m_rawData[0] = (1 - 2 * y * y - 2 * z * z) * scaleX;
		m_rawData[1] = (2 * x * y + 2 * w * z) * scaleX;
		m_rawData[2] = (2 * x * z - 2 * w * y) * scaleX;
		m_rawData[4] = (2 * x * y - 2 * w * z) * scaleY;
		m_rawData[5] = (1 - 2 * x * x - 2 * z * z) * scaleY;
		m_rawData[6] = (2 * y * z + 2 * w * x) * scaleY;
		m_rawData[8] = (2 * x * z + 2 * w * y) * scaleZ;
		m_rawData[9] = (2 * y * z - 2 * w * x) * scaleZ;
		m_rawData[10] = (1 - 2 * x * x - 2 * y * y) * scaleZ;
	}

	m_rawData[3] = m_rawData[7] = m_rawData[11] = 0;
	m_rawData[12] = components[0].m_x;
	m_rawData[13] = components[0].m_y;
	m_rawData[14] = components[0].m_z;
	m_rawData[15] = 1;

	return true;
}

void Matrix3D::decompose(Vector3D* result, Orientation3D orientationStyle) const
{
	float a = m_rawData[0];
	float e = m_rawData[1];
	float i = m_rawData[2];
	float b = m_rawData[4];
	float f = m_rawData[5];
	float j = m_rawData[6];
	float c = m_rawData[8];
	float g = m_rawData[9];
	float k = m_rawData[10];
	float tx = std::sqrt(a * a + e * e + i * i);
	float ty = std::sqrt(b * b + f * f + j * j);
	float tz = std::sqrt(c * c + g * g + k * k);
	float tw = 0;
	float scaleX = tx;
	float scaleY = ty;
	float scaleZ = tz;

	if (a * (f * k - j * g) - e * (b * k - j * c) + i * (b * g - f * c) < 0)
		scaleZ = -scaleZ;

	a /= scaleX;
	e /= scaleX;
	i /= scaleX;
	b /= scaleY;
	f /= scaleY;
	j /= scaleY;
	c /= scaleZ;
	g /= scaleZ;
	k /= scaleZ;

	if (orientationStyle == Orientation3D::EULER_ANGLES)
	{
		tx = std::atan2(j, k);
		ty = std::atan2(-i, std::sqrt(a * a + e * e));
		float s1 = std::sin(tx);
		float c1 = std::cos(tx);
		tz = std::atan2(s1 * c - c1 * b, c1 * f - s1 * g);
	}
	else if (orientationStyle == Orientation3D::AXIS_ANGLE)
	{
		tw = std::acos((a + f + k - 1) / 2);
		float len = std::sqrt((j - g) * (j - g) + (c - i) * (c - i) + (e - b) * (e - b));
		if (len != 0)
		{
			tx = (j - g) / len;
			ty = (c - i) / len;
			tz = (e - b) / len;
		}
		else
		{
			tx = ty = tz = 0;
		}
	}
	else
	{
		float tr = a + f + k;
		if (tr > 0)
		{
			tw = std::sqrt(1 + tr) / 2;
			tr = 4 * tw;
			tx = (j - g) / tr;
			ty = (c - i) / tr;
			tz = (e - b) / tr;
		}
		else if ((a > f) && (a > k))
		{
			tx = std::sqrt(1 + a - f - k) / 2;
			tr = 4 * tx;
			tw = (j - g) / tr;
			ty = (e + b) / tr;
			tz = (c + i) / tr;
		}
		else if (f > k)
		{
			ty = std::sqrt(1 + f - a - k) / 2;
			tr = 4 * ty;
			tx = (e + b) / tr;
			tw = (c - i) / tr;
			tz = (j + g) / tr;
		}
		else
		{
			tz = std::sqrt(1 + k - a - f) / 2;
			tr = 4 * tz;
			tx = (c + i) / tr;
			ty = (j + g) / tr;
			tw = (e - b) / tr;
		}
	}

	result[0].m_x = m_rawData[12];
	result[0].m_y = m_rawData[13];
	result[0].m_z = m_rawData[14];
	result[1].m_x = tx;
	result[1].m_y = ty;
	result[1].m_z = tz;
	result[1].m_w = tw;
	result[2].m_x = scaleX;
	result[2].m_y = scaleY;
	result[2].m_z = scaleZ;
}

void Matrix3D::getForward(Vector3D& result) const
{
	copyColumnTo(2, result);
	result.normalize();
}

void Matrix3D::getUp(Vector3D& result) const
{
	copyColumnTo(1, result);
	result.normalize();
}

void Matrix3D::getRight(Vector3D& result) const
{
	copyColumnTo(0, result);
	result.normalize();
}

void Matrix3D::transformVectors(const float* vin, float* vout, int numPoints) const
{
	for (int i = 0, j = 0; i < numPoints; i++)
	{
		float x = vin[j];
		float y = vin[j + 1];
		float z = vin[j + 2];
		vout[j] = x * m_rawData[0] + y * m_rawData[4] + z * m_rawData[8] + m_rawData[12];
		vout[j + 1] = x * m_rawData[1] + y * m_rawData[5] + z * m_rawData[9] + m_rawData[13];
		vout[j + 2] = x * m_rawData[2] + y * m_rawData[6] + z * m_rawData[10] + m_rawData[14];
		j += 3;
	}
}

void Matrix3D::getAxisRotation(float x, float y, float z, float degrees, Matrix3D& result) const
{
	float rad = degrees * MathConsts::DEGREES_TO_RADIANS;
	float c = std::cos(rad);
	float s = std::sin(rad);
	float t = 1 - c;
	result.m_rawData[0] = c + x * x * t;
	result.m_rawData[5] = c + y * y * t;
	result.m_rawData[10] = c + z * z * t;

	float tmp1 = x * y * t;
	float tmp2 = z * s;
	result.m_rawData[1] = tmp1 + tmp2;
	result.m_rawData[4] = tmp1 - tmp2;

	tmp1 = x * z * t;
	tmp2 = y * s;
	result.m_rawData[8] = tmp1 + tmp2;
	result.m_rawData[2] = tmp1 - tmp2;

	tmp1 = y * z * t;
	tmp2 = x * s;
	result.m_rawData[9] = tmp1 - tmp2;
	result.m_rawData[6] = tmp1 + tmp2;
}

Vector3D* Matrix3D::transformVector(const Matrix3D& matrix, const Vector3D* vin, Vector3D* vout)
{
	float x = vin->m_x;
	float y = vin->m_y;
	float z = vin->m_z;
	const float(&raw)[16] = matrix.m_rawData;
	vout->m_x = raw[0] * x + raw[4] * y + raw[8] * z + raw[12];
	vout->m_y = raw[1] * x + raw[5] * y + raw[9] * z + raw[13];
	vout->m_z = raw[2] * x + raw[6] * y + raw[10] * z + raw[14];
	vout->m_w = raw[3] * x + raw[7] * y + raw[11] * z + raw[15];
	return vout;
}

Vector3D* Matrix3D::deltaTransformVector(const Matrix3D& matrix, const Vector3D* vin, Vector3D* vout)
{
	float x = vin->m_x;
	float y = vin->m_y;
	float z = vin->m_z;
	const float(&raw)[16] = matrix.m_rawData;
	vout->m_x = raw[0] * x + raw[4] * y + raw[8] * z;
	vout->m_y = raw[1] * x + raw[5] * y + raw[9] * z;
	vout->m_z = raw[2] * x + raw[6] * y + raw[10] * z;
	vout->m_w = raw[3] * x + raw[7] * y + raw[11] * z;
	return vout;
}