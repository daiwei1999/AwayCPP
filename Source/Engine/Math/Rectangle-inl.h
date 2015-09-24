template <typename T>
Rectangle<T>::Rectangle(T x, T y, T width, T height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

template <typename T>
void Rectangle<T>::getBottomRight(Point<T>& result) const
{
	result.m_x = m_x + m_width;
	result.m_y = m_y + m_height;
}

template <typename T>
void Rectangle<T>::getSize(Point<T>& result) const
{
	result.m_x = m_width;
	result.m_y = m_height;
}

template <typename T>
void Rectangle<T>::getTopLeft(Point<T>& result) const
{
	result.m_x = m_x;
	result.m_y = m_y;
}

template <typename T>
bool Rectangle<T>::contains(T x, T y) const
{
	return (m_x <= x && m_x + m_width >= x && m_y <= y && m_y + m_height >= y);
}

template <typename T>
bool Rectangle<T>::containsRect(Rectangle<T>& rect) const
{
	return (m_x <= rect.m_x && m_x + m_width >= rect.m_x + rect.m_width && m_y <= rect.m_y && m_y + m_height >= rect.m_y + rect.m_height);
}

template <typename T>
void Rectangle<T>::copyFrom(const Rectangle<T>& sourceRect)
{
	m_x = sourceRect.m_x;
	m_y = sourceRect.m_y;
	m_width = sourceRect.m_width;
	m_height = sourceRect.m_height;
}

template <typename T>
bool Rectangle<T>::equals(Rectangle<T>& toCompare) const
{
	return (m_x == toCompare.m_x && m_y == toCompare.m_y && m_width == toCompare.m_width && m_height == toCompare.m_height);
}

template <typename T>
void Rectangle<T>::inflate(T dx, T dy)
{
	m_x -= dx;
	m_y -= dy;
	m_width += 2 * dx;
	m_height += 2 * dy;
}

template <typename T>
bool Rectangle<T>::intersection(const Rectangle<T>& toIntersect, Rectangle<T>& result) const
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

template <typename T>
bool Rectangle<T>::intersects(const Rectangle<T>& toIntersect) const
{
	return (m_x + m_width > toIntersect.m_x && m_x < toIntersect.m_x + toIntersect.m_width && m_y + m_height > toIntersect.m_y && m_y < toIntersect.m_y + toIntersect.m_height);
}

template <typename T>
void Rectangle<T>::offset(T dx, T dy)
{
	m_x += dx;
	m_y += dy;
}

template <typename T>
void Rectangle<T>::setEmpty()
{
	m_x = m_y = m_width = m_height = 0;
}

template <typename T>
void Rectangle<T>::setTo(T xa, T ya, T widtha, T heighta)
{
	m_x = xa;
	m_y = ya;
	m_width = widtha;
	m_height = heighta;
}

template <typename T>
void Rectangle<T>::merge(const Rectangle<T>& toUnion, Rectangle<T>& result) const
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