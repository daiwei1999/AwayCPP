#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ColorTransform
{
public:
	ColorTransform(float redMultiplier = 1, float greenMultiplier = 1, float blueMultiplier = 1, float alphaMultiplier = 1,
		           float redOffset = 0, float greenOffset = 0, float blueOffset = 0, float alphaOffset = 0);
	void concat(const ColorTransform& second);

public:
	float m_alphaMultiplier;
	float m_alphaOffset;
	float m_blueMultiplier;
	float m_blueOffset;
	float m_greenMultiplier;
	float m_greenOffset;
	float m_redMultiplier;
	float m_redOffset;
};

AWAY_NAMESPACE_END