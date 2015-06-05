#include "GeometryEvent.h"

USING_AWAY_NAMESPACE

const std::string GeometryEvent::SUB_GEOMETRY_ADDED = "SubGeometryAdded";
const std::string GeometryEvent::SUB_GEOMETRY_REMOVED = "SubGeometryRemoved";
const std::string GeometryEvent::BOUNDS_INVALID = "BoundsInvalid";

GeometryEvent::GeometryEvent(const std::string& type, ISubGeometry* subGeometry) : Event(type)
{
	m_subGeometry = subGeometry;
}
