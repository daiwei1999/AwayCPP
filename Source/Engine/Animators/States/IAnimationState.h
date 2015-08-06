#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class IAnimationState
{
public:
	virtual void offset(float startTime) = 0;
	virtual void update(float time) = 0;
	virtual void phase(float value) = 0;
};

AWAY_NAMESPACE_END