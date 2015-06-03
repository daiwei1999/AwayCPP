#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Point
{
public:
	Point(float x = 0, float y = 0);

	float getLength() const;
	void add(const Point& v);
	void subtract(const Point& v);
	void copyFrom(const Point& sourcePoint);
	bool equals(const Point& toCompare) const;
	void normalize(float thickness = 1);
	void offset(float dx, float dy);
	void setTo(float xa, float ya);

	static float distance(const Point& pt1, const Point& pt2);
	static void interpolate(const Point& pt1, const Point& pt2, float f, Point& result);
	static void polar(float len, float angle, Point& result);

public:
	float m_x, m_y;
};

AWAY_NAMESPACE_END