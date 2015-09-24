#include "Quaternion.h"
#include "Vector3D.h"
#include "Matrix3D.h"

USING_AWAY_NAMESPACE

Quaternion::Quaternion(float x, float y, float z, float w)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

float Quaternion::getMagnitude() const
{
	return std::sqrt(m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z);
}

void Quaternion::multiply(const Quaternion& qa, const Quaternion& qb)
{
	float w1 = qa.m_w, x1 = qa.m_x, y1 = qa.m_y, z1 = qa.m_z;
	float w2 = qb.m_w, x2 = qb.m_x, y2 = qb.m_y, z2 = qb.m_z;

	m_w = w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2;
	m_x = w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2;
	m_y = w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2;
	m_z = w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2;
}

void Quaternion::multiplyVector(const Vector3D& vector, Quaternion& result) const
{
	float x2 = vector.m_x;
	float y2 = vector.m_y;
	float z2 = vector.m_z;

	result.m_w = -m_x * x2 - m_y * y2 - m_z * z2;
	result.m_x = m_w * x2 + m_y * z2 - m_z * y2;
	result.m_y = m_w * y2 - m_x * z2 + m_z * x2;
	result.m_z = m_w * z2 + m_x * y2 - m_y * x2;
}

void Quaternion::fromAxisAngle(const Vector3D& axis, float angle)
{
	float halfAngle = angle * .5f;
	float sin_a = std::sin(halfAngle);
	float cos_a = std::cos(halfAngle);

	m_x = axis.m_x * sin_a;
	m_y = axis.m_y * sin_a;
	m_z = axis.m_z * sin_a;
	m_w = cos_a;
	normalize();
}

void Quaternion::slerp(const Quaternion& qa, const Quaternion& qb, float t)
{
	float w1 = qa.m_w, x1 = qa.m_x, y1 = qa.m_y, z1 = qa.m_z;
	float w2 = qb.m_w, x2 = qb.m_x, y2 = qb.m_y, z2 = qb.m_z;
	float dot = w1 * w2 + x1 * x2 + y1 * y2 + z1 * z2;

	// shortest direction
	if (dot < 0)
	{
		dot = -dot;
		w2 = -w2;
		x2 = -x2;
		y2 = -y2;
		z2 = -z2;
	}

	if (dot < 0.95f)
	{
		// interpolate angle linearly
		float angle = std::acos(dot);
		float s = 1 / std::sin(angle);
		float s1 = std::sin(angle * (1 - t)) * s;
		float s2 = std::sin(angle * t) * s;
		m_w = w1 * s1 + w2 * s2;
		m_x = x1 * s1 + x2 * s2;
		m_y = y1 * s1 + y2 * s2;
		m_z = z1 * s1 + z2 * s2;
	}
	else
	{
		// nearly identical angle, interpolate linearly
		m_w = w1 + t * (w2 - w1);
		m_x = x1 + t * (x2 - x1);
		m_y = y1 + t * (y2 - y1);
		m_z = z1 + t * (z2 - z1);

		float len = 1 / std::sqrt(m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z);
		m_w *= len;
		m_x *= len;
		m_y *= len;
		m_z *= len;
	}
}

void Quaternion::lerp(const Quaternion& qa, const Quaternion& qb, float t)
{
	float w1 = qa.m_w, x1 = qa.m_x, y1 = qa.m_y, z1 = qa.m_z;
	float w2 = qb.m_w, x2 = qb.m_x, y2 = qb.m_y, z2 = qb.m_z;

	// shortest direction
	if (w1 * w2 + x1 * x2 + y1 * y2 + z1 * z2 < 0)
	{
		w2 = -w2;
		x2 = -x2;
		y2 = -y2;
		z2 = -z2;
	}

	m_w = w1 + t * (w2 - w1);
	m_x = x1 + t * (x2 - x1);
	m_y = y1 + t * (y2 - y1);
	m_z = z1 + t * (z2 - z1);

	float len = 1 / std::sqrt(m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z);
	m_w *= len;
	m_x *= len;
	m_y *= len;
	m_z *= len;
}

void Quaternion::fromEulerAngles(float ax, float ay, float az)
{
	float halfX = ax * .5f, halfY = ay * .5f, halfZ = az * .5f;
	float cosX = std::cos(halfX), sinX = std::sin(halfX);
	float cosY = std::cos(halfY), sinY = std::sin(halfY);
	float cosZ = std::cos(halfZ), sinZ = std::sin(halfZ);

	m_w = cosX * cosY * cosZ + sinX * sinY * sinZ;
	m_x = sinX * cosY * cosZ - cosX * sinY * sinZ;
	m_y = cosX * sinY * cosZ + sinX * cosY * sinZ;
	m_z = cosX * cosY * sinZ - sinX * sinY * cosZ;
}

void Quaternion::toEulerAngles(Vector3D& result) const
{
	result.m_x = std::atan2(2 * (m_w * m_x + m_y * m_z), 1 - 2 * (m_x * m_x + m_y * m_y));
	result.m_y = std::asin(2 * (m_w * m_y - m_z * m_x));
	result.m_z = std::atan2(2 * (m_w * m_z + m_x * m_y), 1 - 2 * (m_y * m_y + m_z * m_z));
}

void Quaternion::normalize()
{
	float mag = 1 / std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w);
	m_x *= mag;
	m_y *= mag;
	m_z *= mag;
	m_w *= mag;
}

void Quaternion::toMatrix3D(Matrix3D& result) const
{
	float t = 2 * m_x;
	float xy2 = t * m_y;
	float xz2 = t * m_z;
	float xw2 = t * m_w;
	t = 2 * m_y;
	float yz2 = t * m_z;
	float yw2 = t * m_w;
	float zw2 = 2 * m_z * m_w;
	float xx = m_x * m_x;
	float yy = m_y * m_y;
	float zz = m_z * m_z;
	float ww = m_w * m_w;

	t = xx - yy;
	result.m_rawData[0] = t - zz + ww;
	result.m_rawData[4] = xy2 - zw2;
	result.m_rawData[8] = xz2 + yw2;
	result.m_rawData[1] = xy2 + zw2;
	result.m_rawData[5] = -t - zz + ww;
	result.m_rawData[9] = yz2 - xw2;
	result.m_rawData[2] = xz2 - yw2;
	result.m_rawData[6] = yz2 + xw2;
	result.m_rawData[10] = -xx - yy + zz + ww;
	result.m_rawData[3] = result.m_rawData[7] = result.m_rawData[11] = result.m_rawData[12] = result.m_rawData[13] = result.m_rawData[14] = 0;
	result.m_rawData[15] = 1;
}

void Quaternion::fromMatrix3D(const Matrix3D& matrix)
{
	Vector3D v[3];
	matrix.decompose(v, Orientation3D::QUATERNION);
	m_x = v[1].m_x;
	m_y = v[1].m_y;
	m_z = v[1].m_z;
	m_w = v[1].m_w;
}

void Quaternion::rotatePoint(const Vector3D& vector, Vector3D& result) const
{
	float x2 = vector.m_x, y2 = vector.m_y, z2 = vector.m_z;

	// p*q'
	float w1 = m_x * x2 + m_y * y2 + m_z * z2;
	float x1 = m_w * x2 + m_y * z2 - m_z * y2;
	float y1 = m_w * y2 - m_x * z2 + m_z * x2;
	float z1 = m_w * z2 + m_x * y2 - m_y * x2;

	result.m_x = w1 * m_x + x1 * m_w - y1 * m_z + z1 * m_y;
	result.m_y = w1 * m_y + x1 * m_z + y1 * m_w - z1 * m_x;
	result.m_z = w1 * m_z - x1 * m_y + y1 * m_x + z1 * m_w;
}
