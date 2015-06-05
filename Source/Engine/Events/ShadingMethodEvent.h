#pragma once

#include "Common.h"
#include "Event.h"

AWAY_NAMESPACE_BEGIN

class ShadingMethodEvent : public Event
{
public:
	ShadingMethodEvent(const std::string& type);

public:
	static const std::string SHADER_INVALIDATED;
};

AWAY_NAMESPACE_END