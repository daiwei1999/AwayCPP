#include "ColorMaterial.h"

USING_AWAY_NAMESPACE

ColorMaterial::ColorMaterial(unsigned int color, float alpha)
{
	m_diffuseAlpha = 1;
	setColor(color);
	setAlpha(alpha);
}

void ColorMaterial::setAlpha(float value)
{
	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;

	m_diffuseAlpha = value;
	m_screenPass->getDiffuseMethod()->setDiffuseAlpha(value);
	m_screenPass->setBlendMode((m_blendMode == BlendMode::NORMAL && requiresBlending()) ? BlendMode::LAYER : m_blendMode);
}

void ColorMaterial::setColor(unsigned int value)
{
	m_screenPass->getDiffuseMethod()->setDiffuseColor(value);
}

bool ColorMaterial::requiresBlending()
{
	return SinglePassMaterialBase::requiresBlending() || (m_diffuseAlpha < 1);
}