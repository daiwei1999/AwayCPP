#pragma once

#include "Common.h"
#include "FilterBase.h"

AWAY_NAMESPACE_BEGIN

class ColorMatrixFilter : public FilterBase
{
public:
	ColorMatrixFilter(const float matrix[20]);

private:
	ColorMatrixFilterTask* m_colorMatrixFilterTask;
};

AWAY_NAMESPACE_END