#pragma once

#include "Common.h"
#include "EventDispatcher.h"

AWAY_NAMESPACE_BEGIN

/**
 * Geometry is a collection of SubGeometries, each of which contain the actual geometrical data such as vertices,
 * normals, uvs, etc. It also contains a reference to an animation class, which defines how the geometry moves.
 * A Geometry object is assigned to a Mesh, a scene graph occurence of the geometry, which in turn assigns
 * the SubGeometries to its respective SubMesh objects.
 */
class Geometry : public EventDispatcher
{
public:
	Geometry();

	void addSubGeometry(ISubGeometry* subGeometry);
	void removeSubGeometry(ISubGeometry* subGeometry);
	void invalidateBounds(ISubGeometry* subGeometry);
	virtual SubGeometryVector& getSubGeometries() { return m_subGeometries; }

private:
	SubGeometryVector m_subGeometries;
};

AWAY_NAMESPACE_END