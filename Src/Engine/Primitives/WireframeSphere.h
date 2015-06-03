#pragma once

#include "Common.h"
#include "WireframePrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class WireframeSphere : public WireframePrimitiveBase
{
public:
	WireframeSphere(float radius = 50, int segmentsW = 16, int segmentsH = 12, unsigned int color = 0xFFFFFF);

	float getRadius() { return m_radius; }
	void setRadius(float value);
	int getSegmentsW() { return m_segmentsW; }
	void setSegmentsW(int value);
	int getSegmentsH() { return m_segmentsH; }
	void setSegmentsH(int value);

protected:
	void buildGeometry() override;

private:
	float m_radius;
	int m_segmentsW;
	int m_segmentsH;
};

AWAY_NAMESPACE_END