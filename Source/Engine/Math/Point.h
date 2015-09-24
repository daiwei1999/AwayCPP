#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

template <typename T>
class Point
{
public:
	Point(T x = 0, T y = 0);

	float getLength() const;
	void add(const Point<T>& v);
	void subtract(const Point<T>& v);
	void copyFrom(const Point<T>& sourcePoint);
	bool equals(const Point<T>& toCompare) const;
	void normalize(float thickness = 1);
	void offset(T dx, T dy);
	void setTo(T xa, T ya);

	static float distance(const Point<T>& pt1, const Point<T>& pt2);
	static void interpolate(const Point<T>& pt1, const Point<T>& pt2, float f, Point<T>& result);

public:
	T m_x;
	T m_y;
};

#include "Point-inl.h"

AWAY_NAMESPACE_END