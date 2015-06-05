#include "Scene3DEvent.h"

USING_AWAY_NAMESPACE

const std::string Scene3DEvent::PARTITION_CHANGED = "PartitionChanged";

Scene3DEvent::Scene3DEvent(const std::string& type, ObjectContainer3D* objectContainer) : Event(type)
{
	m_objectContainer3D = objectContainer;
}