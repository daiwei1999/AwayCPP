#include "Vector3D.h"

USING_AWAY_NAMESPACE

Vector3D Vector3D::ZERO(0, 0, 0, 1);
Vector3D Vector3D::X_AXIS(1, 0, 0, 0);
Vector3D Vector3D::Y_AXIS(0, 1, 0, 0);
Vector3D Vector3D::Z_AXIS(0, 0, 1, 0);

Vector3D::Vector3D(float x, float y, float z, float w)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

Vector3D::Vector3D(const Vector3D& v)
{
	m_x = v.m_x;
	m_y = v.m_y;
	m_z = v.m_z;
	m_w = v.m_w;
}

void Vector3D::copyFrom(const Vector3D& sourceVector3D)
{
	m_x = sourceVector3D.m_x;
	m_y = sourceVector3D.m_y;
	m_z = sourceVector3D.m_z;
	m_w = sourceVector3D.m_w;
}

void Vector3D::decrementBy(const Vector3D& a)
{
	m_x -= a.m_x;
	m_y -= a.m_y;
	m_z -= a.m_z;
}

void Vector3D::incrementBy(const Vector3D& a)
{
	m_x += a.m_x;
	m_y += a.m_y;
	m_z += a.m_z;
}

void Vector3D::scaleBy(float s)
{
	m_x *= s;
	m_y *= s;
	m_z *= s;
}

void Vector3D::setTo(float xa, float ya, float za)
{
	m_x = xa;
	m_y = ya;
	m_z = za;
}

void Vector3D::setTo(float xa, float ya, float za, float wa)
{
	m_x = xa;
	m_y = ya;
	m_z = za;
	m_w = wa;
}

void Vector3D::add(const Vector3D& a, Vector3D& result) const
{
	result.m_x = m_x + a.m_x;
	result.m_y = m_y + a.m_y;
	result.m_z = m_z + a.m_z;
}

void Vector3D::subtract(const Vector3D& a, Vector3D& result) const
{
	result.m_x = m_x - a.m_x;
	result.m_y = m_y - a.m_y;
	result.m_z = m_z - a.m_z;
}

void Vector3D::crossProduct(const Vector3D& a, Vector3D& result) const
{
	result.m_x = m_y * a.m_z - m_z * a.m_y;
	result.m_y = m_z * a.m_x - m_x * a.m_z;
	result.m_z = m_x * a.m_y - m_y * a.m_x;
	result.m_w = 1;
}

float Vector3D::dotProduct(const Vector3D& a) const
{
	return m_x * a.m_x + m_y * a.m_y + m_z * a.m_z;
}

void Vector3D::negate()
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
}

void Vector3D::normalize()
{
	float length = getLength();
	if (length != 0)
	{
		float invLength = 1 / length;
		m_x *= invLength;
		m_y *= invLength;
		m_z *= invLength;
	}
}

void Vector3D::project()
{
	m_x /= m_w;
	m_y /= m_w;
	m_z /= m_w;
}

float Vector3D::distance(const Vector3D& pt1, const Vector3D& pt2)
{
	float x = pt1.m_x - pt2.m_x;
	float y = pt1.m_y - pt2.m_y;
	float z = pt1.m_z - pt2.m_z;
	return std::sqrt(x * x + y * y + z * z);
}

float Vector3D::angleBetween(const Vector3D& a, const Vector3D& b)
{
	return std::acos(a.dotProduct(b) / (a.getLength() * b.getLength()));
}