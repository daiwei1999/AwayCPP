template <typename T>
Point<T>::Point(T x, T y)
{
	m_x = x;
	m_y = y;
}

template <typename T>
float Point<T>::getLength() const
{
	return std::sqrt(static_cast<float>(m_x * m_x + m_y * m_y));
}

template <typename T>
void Point<T>::add(const Point<T>& v)
{
	m_x += v.m_x;
	m_y += v.m_y;
}

template <typename T>
void Point<T>::subtract(const Point<T>& v)
{
	m_x -= v.m_x;
	m_y -= v.m_y;
}

template <typename T>
void Point<T>::copyFrom(const Point<T>& sourcePoint)
{
	m_x = sourcePoint.m_x;
	m_y = sourcePoint.m_y;
}

template <typename T>
bool Point<T>::equals(const Point<T>& toCompare) const
{
	return (m_x == toCompare.m_x && m_y == toCompare.m_y);
}

template <typename T>
void Point<T>::normalize(float thickness)
{
	float length = getLength();
	if (length != 0)
	{
		float invLength = thickness / length;
		m_x = static_cast<T>(m_x * invLength);
		m_y = static_cast<T>(m_y * invLength);
	}
}

template <typename T>
void Point<T>::offset(T dx, T dy)
{
	m_x += dx;
	m_y += dy;
}

template <typename T>
void Point<T>::setTo(T xa, T ya)
{
	m_x = xa;
	m_y = ya;
}

template <typename T>
float Point<T>::distance(const Point<T>& pt1, const Point<T>& pt2)
{
	float dx = static_cast<float>(pt2.m_x - pt1.m_x);
	float dy = static_cast<float>(pt2.m_y - pt1.m_y);
	return std::sqrt(dx * dx + dy * dy);
}

template <typename T>
void Point<T>::interpolate(const Point<T>& pt1, const Point<T>& pt2, float f, Point<T>& result)
{
	result.m_x = static_cast<T>(pt2.m_x + (pt1.m_x - pt2.m_x) * f);
	result.m_y = static_cast<T>(pt2.m_y + (pt1.m_y - pt2.m_y) * f);
}