#include "BitmapTexture.h"
#include "IContext.h"
#include "Texture.h"
#include "BitmapData.h"
#include "TextureUtils.h"

USING_AWAY_NAMESPACE

BitmapTexture::BitmapTexture(BitmapData* bitmapData, bool generateMipmaps, bool enableDXTEncoding)
{
	setBitmapData(bitmapData);
	m_hasMipmaps = generateMipmaps;
}

void BitmapTexture::setBitmapData(BitmapData* value)
{
	if (m_bitmapData == value)
		return;

	if (!TextureUtils::isBitmapDataValid(value))
		assert(false);

	invalidateContent();
	setSize(value->getWidth(), value->getHeight());

	m_bitmapData = value;
}

TextureBase* BitmapTexture::createTexture(IContext* context)
{
	return context->createTexture(m_width, m_height, m_format, false);
}

void BitmapTexture::uploadContent(TextureBase* texture)
{
	((Texture*)texture)->uploadFromBitmapData(m_bitmapData, 0);
}