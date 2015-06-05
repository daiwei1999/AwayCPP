#pragma once

#include "Common.h"
#include "Event.h"

AWAY_NAMESPACE_BEGIN

class LensEvent : public Event
{
public:
	LensEvent(const std::string& type, LensBase* lens);

	LensBase* getLens() { return m_lens; }

public:
	static const std::string MATRIX_CHANGED;

private:
	LensBase* m_lens;
};

AWAY_NAMESPACE_END