#include "Event.h"

USING_AWAY_NAMESPACE

const std::string Event::CHANGE = "change";
const std::string Event::RESIZE = "resize";

Event::Event(const std::string& type)
{
	m_type = type;
}