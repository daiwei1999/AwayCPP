#pragma once

#include "Common.h"
#include "FilterBase.h"

AWAY_NAMESPACE_BEGIN

class NullFilter : public FilterBase
{
public:
	NullFilter();

private:
	NullFilterTask* m_nullFilterTask;
};

AWAY_NAMESPACE_END