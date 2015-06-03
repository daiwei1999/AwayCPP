#pragma once

#include "Common.h"
#include "PrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class CubeGeometry : public PrimitiveBase
{
public:
	CubeGeometry(float width = 100, float height = 100, float depth = 100, unsigned short segmentsW = 1, unsigned short segmentsH = 1, unsigned short segmentsD = 1, bool tile6 = false);

	float getWidth() { return m_width; }
	void setWidth(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
	float getDepth() { return m_depth; }
	void setDepth(float value);
	bool getTile6() { return m_tile6; }
	void setTile6(bool value);
	unsigned short getSegmentsW() { return m_segmentsW; }
	void setSegmentsW(unsigned short value);
	unsigned short getSegmentsH() { return m_segmentsH; }
	void setSegmentsH(unsigned short value);
	unsigned short getSegmentsD() { return m_segmentsD; }
	void setSegmentsD(unsigned short value);

protected:
	void buildGeometry(CompactSubGeometry* target) override;
	void buildUVs(CompactSubGeometry* target) override;

private:
	float m_width;
	float m_height;
	float m_depth;
	unsigned short m_segmentsW;
	unsigned short m_segmentsH;
	unsigned short m_segmentsD;
	bool m_tile6;
};

AWAY_NAMESPACE_END