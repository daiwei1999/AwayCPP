#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Vector3D
{
public:
	Vector3D(float x = 0, float y = 0, float z = 0, float w = 0);
	Vector3D(const Vector3D& v);

	void copyFrom(const Vector3D& sourceVector3D);
	void decrementBy(const Vector3D& a);
	void incrementBy(const Vector3D& a);
	void scaleBy(float s);
	void setTo(float xa, float ya, float za);

	void add(const Vector3D& a, Vector3D& result) const;
	void subtract(const Vector3D& a, Vector3D& result) const;
	void crossProduct(const Vector3D& a, Vector3D& result) const;
	float dotProduct(const Vector3D& a) const;
	void negate();
	float normalize();
	void project();

	float getLength() const {return std::sqrt(m_x * m_x + m_y * m_y + m_z * m_z);}
	float getLengthSquared() const { return m_x * m_x + m_y * m_y + m_z * m_z; }

	static float distance(const Vector3D& pt1, const Vector3D& pt2);
	static float angleBetween(const Vector3D& a, const Vector3D& b);

public:
	float m_x, m_y, m_z, m_w;
	static Vector3D ZERO, X_AXIS, Y_AXIS, Z_AXIS;
};

AWAY_NAMESPACE_END