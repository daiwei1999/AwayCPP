#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Tex
{
public:
	static const unsigned int D2;
	static const unsigned int CUBE;
	static const unsigned int D3;

	static const unsigned int NOMIP;
	static const unsigned int MIPNONE;
	static const unsigned int MIPNEAREST;
	static const unsigned int MIPLINEAR;

	static const unsigned int NEAREST;
	static const unsigned int LINEAR;
	static const unsigned int ANISOTROPIC2X;
	static const unsigned int ANISOTROPIC4X;
	static const unsigned int ANISOTROPIC8X;
	static const unsigned int ANISOTROPIC16X;

	static const unsigned int CENTROID;
	static const unsigned int SINGLE;
	static const unsigned int IGNORESAMPLER;

	static const unsigned int REPEAT;
	static const unsigned int WRAP;
	static const unsigned int CLAMP;
	static const unsigned int CLAMP_U_REPEAT_V;
	static const unsigned int REPEAT_U_CLAMP_V;

	static const unsigned int RGBA;
	static const unsigned int DXT1;
	static const unsigned int DXT5;
	static const unsigned int VIDEO;

	static unsigned int bias(float bias)
	{
		return (unsigned int)(bias * 8.0f) << 5;
	}
};

AWAY_NAMESPACE_END