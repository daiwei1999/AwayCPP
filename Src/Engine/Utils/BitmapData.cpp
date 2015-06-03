#include "BitmapData.h"
#include "ColorTransform.h"

USING_AWAY_NAMESPACE

BitmapData::BitmapData(int width, int height, bool transparent, unsigned int fillColor) : m_rect(0, 0, width, height)
{
	m_transparent = transparent;
	m_imageData = (unsigned char*)std::malloc(width * height * (transparent ? 4 : 3));
	fillRect(m_rect, fillColor);
}

BitmapData::~BitmapData()
{
	std::free(m_imageData);
}

void BitmapData::colorTransform(Rectangle& rect, ColorTransform& colorTransform)
{
	int x = (int)rect.m_x, y = (int)rect.m_y;
	int width = (int)rect.m_width, height = (int)rect.m_height;

	int index, bpp = m_transparent ? 4 : 3;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			index = (i + x + (j + y) * width) * bpp;
			m_imageData[index] = m_imageData[index] * colorTransform.m_redMultiplier + colorTransform.m_redOffset;
			m_imageData[index + 1] = m_imageData[index + 1] * colorTransform.m_greenMultiplier + colorTransform.m_greenOffset;
			m_imageData[index + 2] = m_imageData[index + 2] * colorTransform.m_blueMultiplier + colorTransform.m_blueOffset;
			if (m_transparent)
				m_imageData[index + 3] = m_imageData[index + 3] * colorTransform.m_alphaMultiplier + colorTransform.m_alphaOffset;
		}
	}
}

void BitmapData::copyChannel(BitmapData& sourceBitmapData, Rectangle& sourceRect, Point& destPoint, unsigned int sourceChannel, unsigned int destChannel)
{

}

void BitmapData::copyPixels(BitmapData& sourceBitmapData, Rectangle& sourceRect, Point& destPoint, BitmapData* alphaBitmapData, Point* alphaPoint, bool mergeAlpha)
{

}

void BitmapData::copyPixelsToByteArray(Rectangle& rect, ByteArray& data)
{

}

void BitmapData::draw(BitmapData& source, Matrix* matrix, bool smoothing)
{

}

void BitmapData::fillRect(Rectangle& rect, unsigned int color)
{
	int x = (int)rect.m_x, y = (int)rect.m_y;
	int width = (int)rect.m_width, height = (int)rect.m_height;

	int index, bpp = m_transparent ? 4 : 3;
	unsigned char a = (color >> 24) & 0xff;
	unsigned char r = (color >> 16) & 0xff;
	unsigned char g = (color >> 8) & 0xff;
	unsigned char b = color & 0xff;
	
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			index = (i + x + (j + y) * width) * bpp;
			m_imageData[index] = r;
			m_imageData[index + 1] = g;
			m_imageData[index + 2] = b;
			if (m_transparent)
				m_imageData[index + 3] = a;
		}
	}
}

unsigned int BitmapData::getPixel(int x, int y)
{
	int index = (x + y * (int)m_rect.m_width) * (m_transparent ? 4 : 3);
	unsigned char r = m_imageData[index];
	unsigned char g = m_imageData[index + 1];
	unsigned char b = m_imageData[index + 2];
	unsigned char a = m_transparent ? m_imageData[index + 3] : 0xff;
	return (a << 24) | (r << 16) | (g << 8) | b;
}

void BitmapData::setPixel(int x, int y, unsigned color)
{
	int index = (x + y * (int)m_rect.m_width) * (m_transparent ? 4 : 3);
	m_imageData[index] = (color >> 16) & 0xff;
	m_imageData[index + 1] = (color >> 8) & 0xff;
	m_imageData[index + 2] = color & 0xff;
	if (m_transparent)
		m_imageData[index + 3] = (color >> 24) & 0xff;
}