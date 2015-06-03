#include "ColorTransform.h"

USING_AWAY_NAMESPACE

ColorTransform::ColorTransform(float redMultiplier, float greenMultiplier, float blueMultiplier, float alphaMultiplier,
	                           float redOffset, float greenOffset, float blueOffset, float alphaOffset)
{
	m_redMultiplier = redMultiplier;
	m_greenMultiplier = greenMultiplier;
	m_blueMultiplier = blueMultiplier;
	m_alphaMultiplier = alphaMultiplier;
	m_redOffset = redOffset;
	m_greenOffset = greenOffset;
	m_blueOffset = blueOffset;
	m_alphaOffset = alphaOffset;
}

void ColorTransform::concat(const ColorTransform& second)
{
	m_redMultiplier += second.m_redMultiplier;
	m_greenMultiplier += second.m_greenMultiplier;
	m_blueMultiplier += second.m_blueMultiplier;
	m_alphaMultiplier += second.m_alphaMultiplier;
}