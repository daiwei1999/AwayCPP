#pragma once

#include "Common.h"
#include "Point.h"

AWAY_NAMESPACE_BEGIN

class Rectangle
{
public:
	Rectangle(float x = 0, float y = 0, float width = 0, float height = 0);

	float getBottom() const { return m_y + m_height; }
	void getBottomRight(Point& result) const;
	float getLeft() const { return m_x; }
	float getRight() const { return m_x + m_width; }
	void getSize(Point& result) const;
	float getTop() const { return m_y; }
	void getTopLeft(Point& result) const;

	bool contains(float x, float y) const;
	bool containsRect(Rectangle& rect) const;
	void copyFrom(const Rectangle& sourceRect);
	bool equals(Rectangle& toCompare) const;
	void inflate(float dx, float dy);
	bool intersection(const Rectangle& toIntersect, Rectangle& result) const;
	bool intersects(const Rectangle& toIntersect) const;
	bool isEmpty() const { return m_x == 0 && m_y == 0 && m_width == 0 && m_height == 0; }
	void offset(float dx, float dy);
	void setEmpty();
	void setTo(float xa, float ya, float widtha, float heighta);
	void merge(const Rectangle& toUnion, Rectangle& result) const;

public:
	float m_x; // The x coordinate of the top-left corner of the rectangle
	float m_y; // The y coordinate of the top-left corner of the rectangle
	float m_width; // The width of the rectangle
	float m_height; // The height of the rectangle
};

AWAY_NAMESPACE_END