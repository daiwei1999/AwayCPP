#pragma once

#include "Common.h"
#include "CylinderGeometry.h"

AWAY_NAMESPACE_BEGIN

class ConeGeometry : public CylinderGeometry
{
public:
	ConeGeometry(float radius = 50, float height = 100, unsigned short segmentsW = 16, unsigned short segmentsH = 1, bool closed = true, bool yUp = true);

	float getRadius() { return m_bottomRadius; }
	void setRadius(float value);
};

AWAY_NAMESPACE_END