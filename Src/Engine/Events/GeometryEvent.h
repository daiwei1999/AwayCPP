#pragma once

#include "Common.h"
#include "Event.h"

AWAY_NAMESPACE_BEGIN

/**
 * Dispatched to notify changes in a geometry object's state.
 */
class GeometryEvent : public Event
{
public:
	GeometryEvent(const std::string& type, ISubGeometry* subGeometry = nullptr);

	ISubGeometry* getSubGeometry() { return m_subGeometry; }

public:
	static const std::string SUB_GEOMETRY_ADDED;
	static const std::string SUB_GEOMETRY_REMOVED;
	static const std::string BOUNDS_INVALID;

private:
	ISubGeometry* m_subGeometry;
};

AWAY_NAMESPACE_END