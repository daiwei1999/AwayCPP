#include "WireframePlane.h"

USING_AWAY_NAMESPACE

WireframePlane::WireframePlane(float width, float height, int segmentsW, int segmentsH, unsigned int color, PlaneAlignment orientation) : WireframePrimitiveBase(color)
{
	m_width = width;
	m_height = height;
	m_segmentsW = segmentsW;
	m_segmentsH = segmentsH;
	m_orientation = orientation;
}

void WireframePlane::setOrientation(PlaneAlignment value)
{
	m_orientation = value;
	invalidateGeometry();
}

void WireframePlane::setWidth(float value)
{
	m_width = value;
	invalidateGeometry();
}

void WireframePlane::setHeight(float value)
{
	m_height = value;
	invalidateGeometry();
}

void WireframePlane::setSegmentsW(int value)
{
	m_segmentsW = value;
	removeAllSegments();
	invalidateGeometry();
}

void WireframePlane::setSegmentsH(int value)
{
	m_segmentsH = value;
	removeAllSegments();
	invalidateGeometry();
}

void WireframePlane::buildGeometry()
{
	int index = 0;
	float hw = m_width * .5f;
	float hh = m_height * .5f;
	Vector3D v0, v1;

	if (m_orientation == PlaneAlignment::XY_AXIS)
	{
		v0.m_y = hh;
		v0.m_z = 0;
		v1.m_y = -hh;
		v1.m_z = 0;

		for (int ws = 0; ws <= m_segmentsW; ws++)
		{
			v0.m_x = v1.m_x = ws * m_width / m_segmentsW - hw;
			updateOrAddSegment(index++, v0, v1);
		}

		v0.m_x = -hw;
		v1.m_x = hw;

		for (int hs = 0; hs <= m_segmentsH; hs++)
		{
			v0.m_y = v1.m_y = hs * m_height / m_segmentsH - hh;
			updateOrAddSegment(index++, v0, v1);
		}
	}
	else if (m_orientation == PlaneAlignment::XZ_AXIS)
	{
		v0.m_z = hh;
		v0.m_y = 0;
		v1.m_z = -hh;
		v1.m_y = 0;

		for (int ws = 0; ws <= m_segmentsW; ++ws)
		{
			v0.m_x = v1.m_x = ws * m_width / m_segmentsW - hw;
			updateOrAddSegment(index++, v0, v1);
		}

		v0.m_x = -hw;
		v1.m_x = hw;

		for (int hs = 0; hs <= m_segmentsH; ++hs)
		{
			v0.m_z = v1.m_z = hs * m_height / m_segmentsH - hh;
			updateOrAddSegment(index++, v0, v1);
		}
	}
	else if (m_orientation == PlaneAlignment::YZ_AXIS)
	{
		v0.m_y = hh;
		v0.m_x = 0;
		v1.m_y = -hh;
		v1.m_x = 0;

		for (int ws = 0; ws <= m_segmentsW; ++ws)
		{
			v0.m_z = v1.m_z = ws * m_width / m_segmentsW - hw;
			updateOrAddSegment(index++, v0, v1);
		}

		v0.m_z = hw;
		v1.m_z = -hw;

		for (int hs = 0; hs <= m_segmentsH; ++hs)
		{
			v0.m_y = v1.m_y = hs * m_height / m_segmentsH - hh;
			updateOrAddSegment(index++, v0, v1);
		}
	}
}