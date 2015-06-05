#pragma once

#include "Common.h"
#include "ColorTransform.h"

AWAY_NAMESPACE_BEGIN

class ColorSegmentPoint
{
public:
	ColorSegmentPoint(float life, ColorTransform& color);

public:
	float m_life;
	ColorTransform m_color;
};

AWAY_NAMESPACE_END