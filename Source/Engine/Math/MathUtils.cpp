#include "MathUtils.h"

USING_AWAY_NAMESPACE

std::default_random_engine MathUtils::m_generator;

int MathUtils::random(int min, int max)
{
	std::uniform_int_distribution<int> dis(min, max);
	return dis(m_generator);
}

float MathUtils::random(float min, float max)
{
	std::uniform_real_distribution<float> dis(min, max);
	return dis(m_generator);
}