#include "TextureMaterial.h"
#include "SuperShaderPass.h"
#include "ColorTransform.h"
#include "BasicDiffuseMethod.h"
#include "BasicAmbientMethod.h"

USING_AWAY_NAMESPACE

TextureMaterial::TextureMaterial(Texture2DBase* texture, bool smooth, bool repeat, bool mipmap)
{
	setTexture(texture);
	setSmooth(smooth);
	setRepeat(repeat);
	setMipmap(mipmap);
}

bool TextureMaterial::getAnimateUVs()
{
	return m_screenPass->getAnimateUVs();
}

void TextureMaterial::setAnimateUVs(bool value)
{
	m_screenPass->setAnimateUVs(value);
}

float TextureMaterial::getAlpha()
{
	return m_screenPass->getColorTransform() ? m_screenPass->getColorTransform()->m_alphaMultiplier : 1;
}

void TextureMaterial::setAlpha(float value)
{
	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;

	if (value != getAlpha())
	{
		if (getColorTransform() == nullptr)
			setColorTransform(new ColorTransform());

		getColorTransform()->m_alphaMultiplier = value;
		m_screenPass->setBlendMode(m_blendMode == BlendMode::NORMAL && requiresBlending() ? BlendMode::LAYER : m_blendMode);
	}
}

Texture2DBase* TextureMaterial::getTexture()
{
	return m_screenPass->getDiffuseMethod()->getTexture();
}

void TextureMaterial::setTexture(Texture2DBase* value)
{
	m_screenPass->getDiffuseMethod()->setTexture(value);
}

Texture2DBase* TextureMaterial::getAmbientTexture()
{
	return m_screenPass->getAmbientMethod()->getTexture();
}

void TextureMaterial::setAmbientTexture(Texture2DBase* value)
{
	m_screenPass->getAmbientMethod()->setTexture(value);
	m_screenPass->getDiffuseMethod()->setUseAmbientTexture(value ? true : false);
}