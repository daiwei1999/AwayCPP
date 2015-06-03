#include "Geometry.h"
#include "ISubGeometry.h"
#include "GeometryEvent.h"

USING_AWAY_NAMESPACE

Geometry::Geometry()
{

}

void Geometry::addSubGeometry(ISubGeometry* subGeometry)
{
	m_subGeometries.push_back(subGeometry);
	subGeometry->setParentGeometry(this);

	if (hasEventListener(GeometryEvent::SUB_GEOMETRY_ADDED))
	{
		GeometryEvent event(GeometryEvent::SUB_GEOMETRY_ADDED, subGeometry);
		dispatchEvent(&event);
	}

	invalidateBounds(subGeometry);
}

void Geometry::removeSubGeometry(ISubGeometry* subGeometry)
{
	m_subGeometries.erase(std::find(m_subGeometries.begin(), m_subGeometries.end(), subGeometry));
	subGeometry->setParentGeometry(nullptr);

	if (hasEventListener(GeometryEvent::SUB_GEOMETRY_REMOVED))
	{
		GeometryEvent event(GeometryEvent::SUB_GEOMETRY_REMOVED, subGeometry);
		dispatchEvent(&event);
	}

	invalidateBounds(subGeometry);
}

void Geometry::invalidateBounds(ISubGeometry* subGeometry)
{
	if (hasEventListener(GeometryEvent::BOUNDS_INVALID))
	{
		GeometryEvent event(GeometryEvent::BOUNDS_INVALID, subGeometry);
		dispatchEvent(&event);
	}
}