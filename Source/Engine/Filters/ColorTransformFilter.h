#pragma once

#include "Common.h"
#include "FilterBase.h"

AWAY_NAMESPACE_BEGIN

class ColorTransformFilter : public FilterBase
{
public:
	ColorTransformFilter(ColorTransform& colorTransform);

	ColorTransform& getColorTransform();
	void setColorTransform(ColorTransform& value);

private:
	ColorTransformFilterTask* m_colorTransformFilterTask;
};

AWAY_NAMESPACE_END