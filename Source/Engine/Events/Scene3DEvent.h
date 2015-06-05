#pragma once

#include "Common.h"
#include "Event.h"

AWAY_NAMESPACE_BEGIN

class Scene3DEvent : public Event
{
public:
	Scene3DEvent(const std::string& type, ObjectContainer3D* objectContainer);

public:
	static const std::string PARTITION_CHANGED;

private:
	ObjectContainer3D* m_objectContainer3D;
};

AWAY_NAMESPACE_END