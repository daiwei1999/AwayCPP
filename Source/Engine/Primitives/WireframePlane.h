#pragma once

#include "Common.h"
#include "WireframePrimitiveBase.h"
#include "Plane3D.h"

AWAY_NAMESPACE_BEGIN

class WireframePlane : public WireframePrimitiveBase
{
public:
	WireframePlane(float width, float height, int segmentsW = 10, int segmentsH = 10, unsigned int color = 0xFFFFFF, PlaneAlignment orientation = PlaneAlignment::XZ_AXIS);

	PlaneAlignment getOrientation() { return m_orientation; }
	void setOrientation(PlaneAlignment value);
	float getWidth() { return m_width; }
	void setWidth(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
	int getSegmentsW() { return m_segmentsW; }
	void setSegmentsW(int value);
	int getSegmentsH() { return m_segmentsH; }
	void setSegmentsH(int value);

protected:
	void buildGeometry() override;

private:
	float m_width;
	float m_height;
	int m_segmentsW;
	int m_segmentsH;
	PlaneAlignment m_orientation;
};

AWAY_NAMESPACE_END