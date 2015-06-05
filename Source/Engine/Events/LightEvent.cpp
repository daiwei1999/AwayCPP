#include "LightEvent.h"

USING_AWAY_NAMESPACE

const std::string LightEvent::CASTS_SHADOW_CHANGE = "CastsShadowChange";

LightEvent::LightEvent(const std::string& type, LightBase* light) : Event(type)
{
	m_light = light;
}