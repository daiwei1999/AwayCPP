#pragma once

#include "Common.h"
#include "Event.h"

AWAY_NAMESPACE_BEGIN

class LightEvent : public Event
{
public:
	LightEvent(const std::string& type, LightBase* light);

	LightBase* getLight() { return m_light; }

public:
	static const std::string CASTS_SHADOW_CHANGE;

private:
	LightBase* m_light;
};

AWAY_NAMESPACE_END