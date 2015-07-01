#pragma once 

#include "Common.h"
#include "PrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

enum class PlaneGeometryMode
{
	CENTER,
	BOTTOM_LEFT,
	BOTTOM_MIDDLE,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_MIDDLE,
	TOP_RIGHT,
	MIDDLE_LEFT,
	MIDDLE_RIGHT
};

class PlaneGeometry : public PrimitiveBase
{
public:
	PlaneGeometry(float width = 100, float height = 100, unsigned short segmentsW = 1, unsigned short segmentsH = 1, bool yUp = true, bool doubleSided = false, PlaneGeometryMode mode = PlaneGeometryMode::CENTER);

	float getWidth() { return m_width; }
	void setWidth(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
	unsigned short getSegmentsW() { return m_segmentsW; }
	void setSegmentsW(unsigned short value);
	unsigned short getSegmentsH() { return m_segmentsH; }
	void setSegmentsH(unsigned short value);
	bool getYUp() { return m_yUp; }
	void setYUp(bool value);
	bool getDoubleSided() { return m_doubleSided; }
	void setDoubleSided(bool value);
	PlaneGeometryMode getMode() { return m_mode; }
	void setMode(PlaneGeometryMode value);

protected:
	void buildGeometry(CompactSubGeometry* target) override;
	void buildUVs(CompactSubGeometry* target) override;

private:
	float m_width;
	float m_height;
	unsigned short m_segmentsW;
	unsigned short m_segmentsH;
	bool m_yUp;
	bool m_doubleSided;
	PlaneGeometryMode m_mode;
};

AWAY_NAMESPACE_END