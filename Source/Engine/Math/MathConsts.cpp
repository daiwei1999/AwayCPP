#include "MathConsts.h"

USING_AWAY_NAMESPACE

const float MathConsts::PI = 3.14159265358979323846264338327950288f;
const float MathConsts::TWO_PI = PI * 2;
const float MathConsts::HALF_PI = PI * .5f;
const float MathConsts::RADIANS_TO_DEGREES = 180 / MathConsts::PI;
const float MathConsts::DEGREES_TO_RADIANS = MathConsts::PI / 180;
const float MathConsts::Infinity = std::numeric_limits<float>::infinity();