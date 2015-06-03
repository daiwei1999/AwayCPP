#pragma once

#include "Common.h"
#include "WireframePrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class WireframeCylinder : public WireframePrimitiveBase
{
public:
	WireframeCylinder(float topRadius = 50, float bottomRadius = 50, float height = 100, int segmentsW = 16, int segmentsH = 1, unsigned int color = 0xFFFFFF);

	float getTopRadius() { return m_topRadius; }
	void setTopRadius(float value);
	float getBottomRadius() { return m_bottomRadius; }
	void setBottomRadius(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);

protected:
	void buildGeometry() override;

private:
	float m_topRadius;
	float m_bottomRadius;
	float m_height;
	int m_segmentsW;
	int m_segmentsH;
};

AWAY_NAMESPACE_END