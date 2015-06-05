#include "Rectangle.h"

USING_AWAY_NAMESPACE

Rectangle::Rectangle(float x, float y, float width, float height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

void Rectangle::getBottomRight(Point& result) const
{
	result.m_x = m_x + m_width;
	result.m_y = m_y + m_height;
}

void Rectangle::getSize(Point& result) const
{
	result.m_x = m_width;
	result.m_y = m_height;
}

void Rectangle::getTopLeft(Point& result) const
{
	result.m_x = m_x;
	result.m_y = m_y;
}

bool Rectangle::contains(float x, float y) const
{
	return m_x <= x && m_x + m_width >= x && m_y <= y && m_y + m_height >= y;
}

bool Rectangle::containsRect(Rectangle& rect) const
{
	return m_x <= rect.m_x && m_x + m_width >= rect.m_x + rect.m_width && m_y <= rect.m_y && m_y + m_height >= rect.m_y + rect.m_height;
}

void Rectangle::copyFrom(const Rectangle& sourceRect)
{
	m_x = sourceRect.m_x;
	m_y = sourceRect.m_y;
	m_width = sourceRect.m_width;
	m_height = sourceRect.m_height;
}

bool Rectangle::equals(Rectangle& toCompare) const
{
	return m_x == toCompare.m_x && m_y == toCompare.m_y && m_width == toCompare.m_width && m_height == toCompare.m_height;
}

void Rectangle::inflate(float dx, float dy)
{
	m_x -= dx;
	m_y -= dy;
	m_width += 2 * dx;
	m_height += 2 * dy;
}

bool Rectangle::intersection(const Rectangle& toIntersect, Rectangle& result) const
{
	if (!intersects(toIntersect))
		return false;

	if (m_x > toIntersect.m_x)
	{
		result.m_x = m_x;
		result.m_width = toIntersect.m_x - m_x + toIntersect.m_width;

		if (result.m_width > m_width)
			result.m_width = m_width;
	}
	else
	{
		result.m_x = toIntersect.m_x;
		result.m_width = m_x - toIntersect.m_x + m_width;

		if (result.m_width > toIntersect.m_width)
			result.m_width = toIntersect.m_width;
	}

	if (m_y > toIntersect.m_y)
	{
		result.m_y = m_y;
		result.m_height = toIntersect.m_y - m_y + toIntersect.m_height;

		if (result.m_height > m_height)
			result.m_height = m_height;
	}
	else
	{
		result.m_y = toIntersect.m_y;
		result.m_height = m_y - toIntersect.m_y + m_height;

		if (result.m_height > toIntersect.m_height)
			result.m_height = toIntersect.m_height;
	}

	return true;
}

bool Rectangle::intersects(const Rectangle& toIntersect) const
{
	return m_x + m_width > toIntersect.m_x && m_x < toIntersect.m_x + toIntersect.m_width && m_y + m_height > toIntersect.m_y && m_y < toIntersect.m_y + toIntersect.m_height;
}

void Rectangle::offset(float dx, float dy)
{
	m_x += dx;
	m_y += dy;
}

void Rectangle::setEmpty()
{
	m_x = m_y = m_width = m_height = 0;
}

void Rectangle::setTo(float xa, float ya, float widtha, float heighta)
{
	m_x = xa;
	m_y = ya;
	m_width = widtha;
	m_height = heighta;
}

void Rectangle::merge(const Rectangle& toUnion, Rectangle& result) const
{
	if (m_x < toUnion.m_x)
	{
		result.m_x = m_x;
		result.m_width = toUnion.m_x - m_x + toUnion.m_width;

		if (result.m_width < m_width)
			result.m_width = m_width;
	}
	else
	{
		result.m_x = toUnion.m_x;
		result.m_width = m_x - toUnion.m_x + m_width;

		if (result.m_width < toUnion.m_width)
			result.m_width = toUnion.m_width;
	}

	if (m_y < toUnion.m_y)
	{
		result.m_y = m_y;
		result.m_height = toUnion.m_y - m_y + toUnion.m_height;

		if (result.m_height < m_height)
			result.m_height = m_height;
	}
	else
	{
		result.m_y = toUnion.m_y;
		result.m_height = m_y - toUnion.m_y + m_height;

		if (result.m_height < toUnion.m_height)
			result.m_height = toUnion.m_height;
	}
}