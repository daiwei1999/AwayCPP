#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class MathUtils
{
public:
	static int random(int min, int max);
	static float random(float min, float max);

private:
	static std::default_random_engine m_generator;
};

AWAY_NAMESPACE_END