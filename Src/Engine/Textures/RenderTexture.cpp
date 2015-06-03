#include "RenderTexture.h"
#include "IContext.h"
#include "Texture.h"
#include "TextureUtils.h"

USING_AWAY_NAMESPACE

RenderTexture::RenderTexture(int width, int height)
{
	setSize(width, height);
}

void RenderTexture::setWidth(int value)
{
	if (value == m_width)
		return;

	if (!TextureUtils::isDimensionValid(value))
		assert(false); // Invalid size: Width and height must be power of 2 and cannot exceed 2048

	invalidateContent();
	setSize(value, m_height);
}

void RenderTexture::setHeight(int value)
{
	if (value == m_height)
		return;

	if (!TextureUtils::isDimensionValid(value))
		assert(false); // Invalid size: Width and height must be power of 2 and cannot exceed 2048

	invalidateContent();
	setSize(m_width, value);
}

TextureBase* RenderTexture::createTexture(IContext* context)
{
	return context->createTexture(m_width, m_height, TextureFormat::BGRA, true);
}

void RenderTexture::uploadContent(TextureBase* texture)
{

}