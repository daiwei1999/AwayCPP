#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Quaternion
{
public:
	Quaternion(float x = 0, float y = 0, float z = 0, float w = 1);

	float getMagnitude() const;
	void multiply(const Quaternion& qa, const Quaternion& qb);
	void multiplyVector(const Vector3D& vector, Quaternion& result) const;
	void fromAxisAngle(const Vector3D& axis, float angle);

	void slerp(const Quaternion& qa, const Quaternion& qb, float t);
	void lerp(const Quaternion& qa, const Quaternion& qb, float t);
	void fromEulerAngles(float ax, float ay, float az);
	void toEulerAngles(Vector3D& result) const;
	void normalize();

	void toMatrix3D(Matrix3D& result) const;
	void fromMatrix3D(const Matrix3D& matrix);

	void rotatePoint(const Vector3D& vector, Vector3D& result) const;

public:
	float m_x, m_y, m_z, m_w;
};

AWAY_NAMESPACE_END