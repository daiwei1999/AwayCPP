#pragma once

#include "Common.h"
#include "Rectangle.h"

AWAY_NAMESPACE_BEGIN

class BitmapData
{
public:
	BitmapData(int width, int height, bool transparent = true, unsigned int fillColor = 0xFFFFFFFF);
	~BitmapData();

	Rectangle& getRect() { return m_rect; }
	int getWidth() { return (int)m_rect.m_width; }
	int getHeight() { return (int)m_rect.m_height; }
	bool isTransparent() { return m_transparent; }

	void colorTransform(Rectangle& rect, ColorTransform& colorTransform);
	void copyChannel(BitmapData& sourceBitmapData, Rectangle& sourceRect, Point& destPoint, unsigned int sourceChannel, unsigned int destChannel);
	void copyPixels(BitmapData& sourceBitmapData, Rectangle& sourceRect, Point& destPoint, BitmapData* alphaBitmapData = nullptr, Point* alphaPoint = nullptr, bool mergeAlpha = false);
	void copyPixelsToByteArray(Rectangle& rect, ByteArray& data);
	void draw(BitmapData& source, Matrix* matrix = nullptr, bool smoothing = false);
	void fillRect(Rectangle& rect, unsigned int color);
	unsigned int getPixel(int x, int y);
	void setPixel(int x, int y, unsigned color);
	void* getImageData() { return m_imageData; }

private:
	Rectangle m_rect;
	bool m_transparent;
	unsigned char* m_imageData;
};

AWAY_NAMESPACE_END