#include "Plane3D.h"
#include "Vector3D.h"

USING_AWAY_NAMESPACE

Plane3D::Plane3D(float a, float b, float c, float d)
{
	m_a = a;
	m_b = b;
	m_c = c;
	m_d = d;

	if (a == 0 && b == 0)
		m_alignment = PlaneAlignment::XY_AXIS;
	else if (b == 0 && c == 0)
		m_alignment = PlaneAlignment::YZ_AXIS;
	else if (a == 0 && c == 0)
		m_alignment = PlaneAlignment::XZ_AXIS;
	else
		m_alignment = PlaneAlignment::ANY;
}

void Plane3D::fromPoints(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2)
{
	float d1x = p1.m_x - p0.m_x;
	float d1y = p1.m_y - p0.m_y;
	float d1z = p1.m_z - p0.m_z;
	float d2x = p2.m_x - p0.m_x;
	float d2y = p2.m_y - p0.m_y;
	float d2z = p2.m_z - p0.m_z;

	m_a = d1y * d2z - d1z * d2y;
	m_b = d1z * d2x - d1x * d2z;
	m_c = d1x * d2y - d1y * d2x;
	m_d = m_a * p0.m_x + m_b * p0.m_y + m_c * p0.m_z;

	// not using epsilon, since a plane is infinite and a small incorrection can grow very large
	if (m_a == 0 && m_b == 0)
		m_alignment = PlaneAlignment::XY_AXIS;
	else if (m_b == 0 && m_c == 0)
		m_alignment = PlaneAlignment::YZ_AXIS;
	else if (m_a == 0 && m_c == 0)
		m_alignment = PlaneAlignment::XZ_AXIS;
	else
		m_alignment = PlaneAlignment::ANY;
}

void Plane3D::fromNormalAndPoint(const Vector3D& normal, const Vector3D& point)
{
	m_a = normal.m_x;
	m_b = normal.m_y;
	m_c = normal.m_z;
	m_d = m_a * point.m_x + m_b * point.m_y + m_c * point.m_z;
	if (m_a == 0 && m_b == 0)
		m_alignment = PlaneAlignment::XY_AXIS;
	else if (m_b == 0 && m_c == 0)
		m_alignment = PlaneAlignment::YZ_AXIS;
	else if (m_a == 0 && m_c == 0)
		m_alignment = PlaneAlignment::XZ_AXIS;
	else
		m_alignment = PlaneAlignment::ANY;
}

void Plane3D::normalize()
{
	float len = 1 / std::sqrt(m_a * m_a + m_b * m_b + m_c * m_c);
	m_a *= len;
	m_b *= len;
	m_c *= len;
	m_d *= len;
}

float Plane3D::distance(const Vector3D& p) const
{
	if (m_alignment == PlaneAlignment::YZ_AXIS)
		return m_a * p.m_x - m_d;
	else if (m_alignment == PlaneAlignment::XZ_AXIS)
		return m_b * p.m_y - m_d;
	else if (m_alignment == PlaneAlignment::XY_AXIS)
		return m_c * p.m_z - m_d;
	else
		return m_a * p.m_x + m_b * p.m_y + m_c * p.m_z - m_d;
}
