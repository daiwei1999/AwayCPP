#include "Point.h"

USING_AWAY_NAMESPACE

Point::Point(float x, float y)
{
	m_x = x;
	m_y = y;
}

float Point::getLength() const
{
	return std::sqrt(m_x * m_x + m_y * m_y);
}

void Point::add(const Point& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
}

void Point::subtract(const Point& v)
{
	m_x -= v.m_x;
	m_y -= v.m_y;
}

void Point::copyFrom(const Point& sourcePoint)
{
	m_x = sourcePoint.m_x;
	m_y = sourcePoint.m_y;
}

bool Point::equals(const Point& toCompare) const
{
	return (m_x == toCompare.m_x && m_y == toCompare.m_y);
}

void Point::normalize(float thickness)
{
	float length = getLength();
	if (length != 0)
	{
		float invLength = thickness / length;
		m_x *= invLength;
		m_y *= invLength;
	}
}

void Point::offset(float dx, float dy)
{
	m_x += dx;
	m_y += dy;
}

void Point::setTo(float xa, float ya)
{
	m_x = xa;
	m_y = ya;
}

float Point::distance(const Point& pt1, const Point& pt2)
{
	float dx = pt2.m_x - pt1.m_x;
	float dy = pt2.m_y - pt1.m_y;
	return std::sqrt(dx * dx + dy * dy);
}

void Point::interpolate(const Point& pt1, const Point& pt2, float f, Point& result)
{
	result.m_x = pt2.m_x + (pt1.m_x - pt2.m_x) * f;
	result.m_y = pt2.m_y + (pt1.m_y - pt2.m_y) * f;
}

void Point::polar(float len, float angle, Point& result)
{
	result.m_x = len * std::cos(angle);
	result.m_y = len * std::sin(angle);
}