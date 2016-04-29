#include "NullFilter.h"
#include "NullFilterTask.h"

USING_AWAY_NAMESPACE

NullFilter::NullFilter()
{
	addTask(m_nullFilterTask = new NullFilterTask());
}