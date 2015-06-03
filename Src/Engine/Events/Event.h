#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Event
{
public:
	Event(const std::string& type);

	std::string& getType() { return m_type; }

public:
	static const std::string CHANGE;

private:
	std::string m_type;
};

AWAY_NAMESPACE_END