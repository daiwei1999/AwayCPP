#pragma once

#include "Common.h"
#include "PrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class SphereGeometry : public PrimitiveBase
{
public:
	SphereGeometry(float radius = 50, unsigned short segmentsW = 16, unsigned short segmentsH = 12, bool yUp = true);

	float getRadius() { return m_radius; }
	void setRadius(float value);
	unsigned short getSegmentsW() { return m_segmentsW; }
	void setSegmentsW(unsigned short value);
	unsigned short getSegmentsH() { return m_segmentsH; }
	void setSegmentsH(unsigned short value);
	bool getYUp() { return m_yUp; }
	void setYUp(bool value);

protected:
	void buildGeometry(CompactSubGeometry* target) override;
	void buildUVs(CompactSubGeometry* target) override;

private:
	float m_radius;
	unsigned short m_segmentsW;
	unsigned short m_segmentsH;
	bool m_yUp;
};

AWAY_NAMESPACE_END