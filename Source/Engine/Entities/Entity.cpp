#include "Entity.h"
#include "BoundingVolumeBase.h"
#include "WireframePrimitiveBase.h"
#include "Scene3D.h"
#include "ControllerBase.h"

USING_AWAY_NAMESPACE

Entity::Entity() : m_pickingCollisionVO(this)
{
	m_showBounds = false;
	m_partitionNode = nullptr;
	m_boundsIsShown = false;
	m_boundsInvalid = true;
	m_worldBoundsInvalid = true;
	m_zOffset = 0;
	m_controller = nullptr;
}

bool Entity::collidesBefore(float shortestCollisionDistance, bool findClosest)
{
	return true;
}

void Entity::setShowBounds(bool value)
{
	if (value == m_showBounds)
		return;

	m_showBounds = value;
	if (m_showBounds)
		addBounds();
	else
		removeBounds();
}

float Entity::getMinX()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds->getMin()->m_x;
}

float Entity::getMinY()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds->getMin()->m_y;
}

float Entity::getMinZ()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds->getMin()->m_z;
}

float Entity::getMaxX()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds->getMax()->m_x;
}

float Entity::getMaxY()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds->getMax()->m_y;
}

float Entity::getMaxZ()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds->getMax()->m_z;
}

BoundingVolumeBase* Entity::getBounds()
{
	if (m_boundsInvalid)
		updateBounds();

	return m_bounds;
}

void Entity::setBounds(BoundingVolumeBase* value)
{
	removeBounds();
	m_bounds->copyFrom(value);
	m_boundsInvalid = false;
	m_worldBoundsInvalid = true;
	notifySceneBoundsInvalid();
	if (m_showBounds)
		addBounds();
}

BoundingVolumeBase* Entity::getWorldBounds()
{
	if (m_worldBoundsInvalid)
		updateWorldBounds();

	return m_worldBounds;
}

void Entity::updateWorldBounds()
{
	m_worldBounds->transformFrom(getBounds(), getSceneTransform());
	m_worldBoundsInvalid = false;
}

void Entity::setImplicitPartition(Partition3D* value)
{
	if (value == m_implicitPartition)
		return;

	if (m_implicitPartition)
		notifyPartitionUnassigned();

	ObjectContainer3D::setImplicitPartition(value);
	notifyPartitionAssigned();
}

void Entity::setScene(Scene3D* value)
{
	if (value == m_scene)
		return;

	if (m_scene)
		m_scene->unregisterEntity(this);

	if (value)
		value->registerEntity(this);

	ObjectContainer3D::setScene(value);
}

EntityNode* Entity::getEntityPartitionNode()
{
	if (!m_partitionNode)
		m_partitionNode = createEntityPartitionNode();

	return m_partitionNode;
}

bool Entity::isIntersectingRay(Vector3D* rayPosition, Vector3D* rayDirection)
{
	// convert ray to entity space
	Matrix3D& inverseSceneTransform = getInverseSceneTransform();
	Matrix3D::transformVector(inverseSceneTransform, rayPosition, &m_pickingCollisionVO.m_localRayPosition);
	Matrix3D::deltaTransformVector(inverseSceneTransform, rayDirection, &m_pickingCollisionVO.m_localRayDirection);

	// check for ray-bounds collision
	float rayEntryDistance = getBounds()->rayIntersection(m_pickingCollisionVO.m_localRayPosition, m_pickingCollisionVO.m_localRayDirection, m_pickingCollisionVO.m_localNormal);
	if (rayEntryDistance < 0)
		return false;

	// Store collision data
	m_pickingCollisionVO.m_rayEntryDistance = rayEntryDistance;
	m_pickingCollisionVO.m_rayPosition.copyFrom(*rayPosition);
	m_pickingCollisionVO.m_rayDirection.copyFrom(*rayDirection);
	m_pickingCollisionVO.m_rayOriginIsInsideBounds = (rayEntryDistance == 0);
	return true;
}

void Entity::invalidateSceneTransform()
{
	if (!m_ignoreTransform)
	{
		ObjectContainer3D::invalidateSceneTransform();
		m_worldBoundsInvalid = true;
		notifySceneBoundsInvalid();
	}
}

void Entity::invalidateBounds()
{
	m_boundsInvalid = true;
	m_worldBoundsInvalid = true;
	notifySceneBoundsInvalid();
}

void Entity::updateMouseChildren()
{

}

void Entity::notifySceneBoundsInvalid()
{
	if (m_scene)
		m_scene->invalidateEntityBounds(this);
}

void Entity::notifyPartitionAssigned()
{
	if (m_scene)
		m_scene->registerPartition(this);
}

void Entity::notifyPartitionUnassigned()
{
	if (m_scene)
		m_scene->unregisterPartition(this);
}

void Entity::addBounds()
{
	if (!m_boundsIsShown)
	{
		m_boundsIsShown = true;
		addChild(m_bounds->getBoundingRenderable());
	}
}

void Entity::removeBounds()
{
	if (m_boundsIsShown)
	{
		m_boundsIsShown = false;
		removeChild(m_bounds->getBoundingRenderable());
		m_bounds->disposeRenderable();
	}
}

void Entity::internalUpdate()
{
	if (m_controller)
		m_controller->update();
}