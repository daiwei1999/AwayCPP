#pragma once

#include "Common.h"
#include "PrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class CapsuleGeometry : public PrimitiveBase
{
public:
	CapsuleGeometry(float radius = 50, float height = 100, unsigned short segmentsW = 16, unsigned short segmentsH = 15, bool yUp = true);

	float getRadius() { return m_radius; }
	void setRadius(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
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
	float m_height;
	unsigned short m_segmentsW;
	unsigned short m_segmentsH;
	bool m_yUp;
};

AWAY_NAMESPACE_END