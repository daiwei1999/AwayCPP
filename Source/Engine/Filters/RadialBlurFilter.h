#pragma once

#include "Common.h"
#include "FilterBase.h"

AWAY_NAMESPACE_BEGIN

class RadialBlurFilter : public FilterBase
{
public:
	RadialBlurFilter(float intensity = 8, float glowGamma = 1.6f, float blurStart = 1, float blurWidth = -0.3f, float cx = 0.5f, float cy = 0.5f);

	float getIntensity();
	void setIntensity(float value);
	float getGlowGamma();
	void setGlowGamma(float value);
	float getBlurStart();
	void setBlurStart(float value);
	float getBlurWidth();
	void setBlurWidth(float value);
	float getCx();
	void setCx(float value);
	float getCy();
	void setCy(float value);

private:
	RadialBlurFilterTask* m_blurTask;
};

AWAY_NAMESPACE_END