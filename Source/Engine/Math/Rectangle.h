#pragma once

#include "Common.h"
#include "Point.h"

AWAY_NAMESPACE_BEGIN

template <typename T>
class Rectangle
{
public:
	Rectangle(T x = 0, T y = 0, T width = 0, T height = 0);

	T getBottom() const { return m_y + m_height; }
	void getBottomRight(Point<T>& result) const;
	T getLeft() const { return m_x; }
	T getRight() const { return m_x + m_width; }
	void getSize(Point<T>& result) const;
	T getTop() const { return m_y; }
	void getTopLeft(Point<T>& result) const;

	bool contains(T x, T y) const;
	bool containsRect(Rectangle<T>& rect) const;
	void copyFrom(const Rectangle<T>& sourceRect);
	bool equals(Rectangle<T>& toCompare) const;
	void inflate(T dx, T dy);
	bool intersection(const Rectangle<T>& toIntersect, Rectangle<T>& result) const;
	bool intersects(const Rectangle<T>& toIntersect) const;
	bool isEmpty() const { return m_x == 0 && m_y == 0 && m_width == 0 && m_height == 0; }
	void offset(T dx, T dy);
	void setEmpty();
	void setTo(T xa, T ya, T widtha, T heighta);
	void merge(const Rectangle<T>& toUnion, Rectangle<T>& result) const;

public:
	T m_x; // The x coordinate of the top-left corner of the rectangle
	T m_y; // The y coordinate of the top-left corner of the rectangle
	T m_width; // The width of the rectangle
	T m_height; // The height of the rectangle
};

#include "Rectangle-inl.h"

AWAY_NAMESPACE_END