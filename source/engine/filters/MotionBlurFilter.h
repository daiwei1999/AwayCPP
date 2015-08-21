#pragma once

#include "Common.h"
#include "FilterBase.h"

AWAY_NAMESPACE_BEGIN

class MotionBlurFilter : public FilterBase
{
public:
	MotionBlurFilter(float strength = 0.65);

	float getStrength();
	void setStrength(float value);

	void update(IContext* context, Camera3D* camera) override;

private:
	XFadeCompositeFilterTask* m_compositeTask;
	DoubleBufferCopyFilterTask* m_copyTask;
};

AWAY_NAMESPACE_END