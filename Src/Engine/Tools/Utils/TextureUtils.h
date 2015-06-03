#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class TextureUtils
{
public:
	static bool isBitmapDataValid(BitmapData* bitmapData);
	static bool isDimensionValid(int d);
	static bool isPowerOfTwo(int value);
	static int getBestPowerOf2(int value);

public:
	static int MAX_SIZE;
};

AWAY_NAMESPACE_END