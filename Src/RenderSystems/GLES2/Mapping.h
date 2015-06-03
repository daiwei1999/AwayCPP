#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class OpLUT
{
public:
	std::string m_format;
	bool m_dest;
	bool m_a;
	bool m_b;
	unsigned char m_matrixHeight;
	bool m_ndwm;
	bool m_scalar;
};

class Mapping
{
public:
	static OpLUT agal2glsllut[46];
};

AWAY_NAMESPACE_END