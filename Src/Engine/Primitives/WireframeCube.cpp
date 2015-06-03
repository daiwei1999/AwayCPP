#include "WireframeCube.h"

USING_AWAY_NAMESPACE

WireframeCube::WireframeCube(float width, float height, float depth, unsigned int color) : WireframePrimitiveBase(color)
{
	m_width = width;
	m_height = height;
	m_depth = depth;
}

void WireframeCube::setWidth(float value)
{
	m_width = value;
	invalidateGeometry();
}

void WireframeCube::setHeight(float value)
{
	m_height = value;
	invalidateGeometry();
}

void WireframeCube::setDepth(float value)
{
	m_depth = value;
	invalidateGeometry();
}

void WireframeCube::buildGeometry()
{
	float hw = m_width * .5f;
	float hh = m_height * .5f;
	float hd = m_depth * .5f;

	Vector3D v0, v1;
	v0.m_x = -hw;
	v0.m_y = hh;
	v0.m_z = -hd;
	v1.m_x = -hw;
	v1.m_y = -hh;
	v1.m_z = -hd;

	updateOrAddSegment(0, v0, v1);
	v0.m_z = hd;
	v1.m_z = hd;
	updateOrAddSegment(1, v0, v1);
	v0.m_x = hw;
	v1.m_x = hw;
	updateOrAddSegment(2, v0, v1);
	v0.m_z = -hd;
	v1.m_z = -hd;
	updateOrAddSegment(3, v0, v1);

	v0.m_x = -hw;
	v0.m_y = -hh;
	v0.m_z = -hd;
	v1.m_x = hw;
	v1.m_y = -hh;
	v1.m_z = -hd;
	updateOrAddSegment(4, v0, v1);
	v0.m_y = hh;
	v1.m_y = hh;
	updateOrAddSegment(5, v0, v1);
	v0.m_z = hd;
	v1.m_z = hd;
	updateOrAddSegment(6, v0, v1);
	v0.m_y = -hh;
	v1.m_y = -hh;
	updateOrAddSegment(7, v0, v1);

	v0.m_x = -hw;
	v0.m_y = -hh;
	v0.m_z = -hd;
	v1.m_x = -hw;
	v1.m_y = -hh;
	v1.m_z = hd;
	updateOrAddSegment(8, v0, v1);
	v0.m_y = hh;
	v1.m_y = hh;
	updateOrAddSegment(9, v0, v1);
	v0.m_x = hw;
	v1.m_x = hw;
	updateOrAddSegment(10, v0, v1);
	v0.m_y = -hh;
	v1.m_y = -hh;
	updateOrAddSegment(11, v0, v1);
}