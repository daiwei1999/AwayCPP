#pragma once

#include "Common.h"
#include "Vector3D.h"
#include "Point.h"

AWAY_NAMESPACE_BEGIN

/**
 * Value object for a picking collision returned by a picking collider. Created as unique objects on entities
 */
class PickingCollisionVO
{
public:
	PickingCollisionVO(Entity* entity) { m_entity = entity; }

public:
	Entity* m_entity; // The entity to which this collision object belongs.
	Vector3D m_localPosition; // The local position of the collision on the entity's surface.
	Vector3D m_localNormal; // The local normal vector at the position of the collision.
	Point m_uv; // The uv coordinate at the position of the collision.
	unsigned int m_index; // The index of the face where the event took place.
	unsigned int m_subGeometryIndex; // The index of the subGeometry where the event took place.
	Vector3D m_localRayPosition; // The starting position of the colliding ray in local coordinates.
	Vector3D m_localRayDirection; // The direction of the colliding ray in local coordinates.
	Vector3D m_rayPosition; // The starting position of the colliding ray in scene coordinates.
	Vector3D m_rayDirection; // The direction of the colliding ray in scene coordinates.
	bool m_rayOriginIsInsideBounds; // Determines if the ray position is contained within the entity bounds.
	float m_rayEntryDistance; // The distance along the ray from the starting position to the calculated intersection entry point with the entity.
	IRenderable* m_renderable; // The IRenderable associated with a collision.
};

AWAY_NAMESPACE_END