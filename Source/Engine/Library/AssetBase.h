#pragma once

#include "Common.h"
#include "EventDispatcher.h"

AWAY_NAMESPACE_BEGIN

class AssetBase : public EventDispatcher
{
public:
	AssetBase();

	std::string& getName() { return m_name; }
	void setName(const std::string& name) { m_name = name; }

private:
	std::string m_name;
};

AWAY_NAMESPACE_END