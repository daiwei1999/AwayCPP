#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * EntitySorterBase provides an abstract base class to sort geometry information in an EntityCollector object for
 * rendering.
 */
class IEntitySorter
{
public:
	virtual void sort(EntityCollector* collector) = 0;
};

AWAY_NAMESPACE_END