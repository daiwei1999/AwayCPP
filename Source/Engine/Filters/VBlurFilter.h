#pragma once

#include "Common.h"
#include "FilterBase.h"

AWAY_NAMESPACE_BEGIN

class VBlurFilter : public FilterBase
{
public:
	VBlurFilter(float amount, int stepSize = -1);

	float getAmount();
	void setAmount(float value);
	int getStepSize();
	void setStepSize(int value);

private:
	VBlurFilterTask* m_blurTask;
};

AWAY_NAMESPACE_END