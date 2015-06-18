#pragma once

#include "Common.h"
#include "SegmentSet.h"

AWAY_NAMESPACE_BEGIN

class Trident : public SegmentSet
{
public:
	Trident(float length = 1000, bool showLetters = true);

private:
	void buildTrident(float length, bool showLetters);
};

AWAY_NAMESPACE_END