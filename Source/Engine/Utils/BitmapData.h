#pragma once

#include "Common.h"
#include "Rectangle.h"

AWAY_NAMESPACE_BEGIN

class BitmapData
{
public:
	BitmapData(int width, int height, bool transparent = true, unsigned int fillColor = 0xFFFFFFFF);
	~BitmapData();

	Rectangle<int>& getRect() { return m_rect; }
	int getWidth() { return (int)m_rect.m_width; }
	int getHeight() { return (int)m_rect.m_height; }
	bool isTransparent() { return m_transparent; }

	void colorTransform(Rectangle<int>& rect, ColorTransform& colorTransform);
	void copyChannel(BitmapData& sourceBitmapData, Rectangle<int>& sourceRect, Point<int>& destPoint, unsigned int sourceChannel, unsigned int destChannel);
	void copyPixels(BitmapData& sourceBitmapData, Rectangle<int>& sourceRect, Point<int>& destPoint, BitmapData* alphaBitmapData = nullptr, Point<int>* alphaPoint = nullptr, bool mergeAlpha = false);
	void copyPixelsToByteArray(Rectangle<int>& rect, ByteArray& data);
	void draw(BitmapData& source, Matrix* matrix = nullptr, bool smoothing = false);
	void fillRect(Rectangle<int>& rect, unsigned int color);
	unsigned int getPixel(int x, int y);
	void setPixel(int x, int y, unsigned int color);
	void* getImageData() { return m_imageData; }

private:
	Rectangle<int> m_rect;
	bool m_transparent;
	unsigned char* m_imageData;
};

AWAY_NAMESPACE_END