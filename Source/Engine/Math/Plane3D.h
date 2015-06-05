#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

enum class PlaneAlignment
{
	ANY,
	XY_AXIS,
	YZ_AXIS,
	XZ_AXIS
};

class Plane3D
{
public:
	Plane3D(float a = 0, float b = 0, float c = 0, float d = 0);

	void fromPoints(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2);
	void fromNormalAndPoint(const Vector3D& normal, const Vector3D& point);
	void normalize();
	float distance(const Vector3D& p) const;

public:
	float m_a; // The A coefficient of this plane. (Also the x dimension of the plane normal)
	float m_b; // The B coefficient of this plane. (Also the y dimension of the plane normal)
	float m_c; // The C coefficient of this plane. (Also the z dimension of the plane normal)
	float m_d; // The D coefficient of this plane. (Also the inverse dot product between normal and point)

private:
	PlaneAlignment m_alignment;
};

AWAY_NAMESPACE_END