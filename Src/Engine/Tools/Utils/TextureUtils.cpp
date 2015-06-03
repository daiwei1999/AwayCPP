#include "TextureUtils.h"
#include "BitmapData.h"

USING_AWAY_NAMESPACE

int TextureUtils::MAX_SIZE = 2048;

bool TextureUtils::isBitmapDataValid(BitmapData* bitmapData)
{
	return isDimensionValid(bitmapData->getWidth()) && isDimensionValid(bitmapData->getHeight());
}

bool TextureUtils::isDimensionValid(int d)
{
	return d >= 1 && d <= MAX_SIZE && isPowerOfTwo(d);
}

bool TextureUtils::isPowerOfTwo(int value)
{
	return value ? ((value & -value) == value) : false;
}

int TextureUtils::getBestPowerOf2(int value)
{
	int p = 1;
	while (p < value)
		p <<= 1;

	if (p > MAX_SIZE)
		p = MAX_SIZE;

	return p;
}