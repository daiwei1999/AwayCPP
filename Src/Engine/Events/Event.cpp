#include "Event.h"

USING_AWAY_NAMESPACE

const std::string Event::CHANGE = "change";

Event::Event(const std::string& type)
{
	m_type = type;
}