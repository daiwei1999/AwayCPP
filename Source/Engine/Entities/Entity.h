#pragma once

#include "Common.h"
#include "ObjectContainer3D.h"
#include "PickingCollisionVO.h"

AWAY_NAMESPACE_BEGIN

/**
 * The Entity class provides an abstract base class for all scene graph objects that are considered having a
 * "presence" in the scene, in the sense that it can be considered an actual object with a position and a size (even
 * if infinite or idealised), rather than a grouping.
 * Entities can be partitioned in a space partitioning system and in turn collected by an EntityCollector.
 */
class Entity : public ObjectContainer3D
{
public:
	Entity();

	bool getShowBounds() { return m_showBounds; };
	void setShowBounds(bool value);

	float getMinX() override;
	float getMinY() override;
	float getMinZ() override;
	float getMaxX() override;
	float getMaxY() override;
	float getMaxZ() override;

	float getZOffset() { return m_zOffset; }
	void setZOffset(float value) { m_zOffset = value; }

	virtual BoundingVolumeBase* getBounds();
	void setBounds(BoundingVolumeBase* value);
	BoundingVolumeBase* getWorldBounds();

	void internalUpdate();
	void setController(ControllerBase* value) { m_controller = value; }
	void setScene(Scene3D* value) override;
	
	EntityNode* getEntityPartitionNode();
	bool isIntersectingRay(Vector3D* rayPosition, Vector3D* rayDirection);
	virtual bool collidesBefore(float shortestCollisionDistance, bool findClosest);
	void setImplicitPartition(Partition3D* value) override;

protected:
	virtual EntityNode* createEntityPartitionNode() = 0;
	virtual void updateBounds() = 0;
	void invalidateSceneTransform() override;
	virtual void invalidateBounds();
	void updateMouseChildren() override;

private:
	void updateWorldBounds();
	void notifySceneBoundsInvalid();
	void notifyPartitionAssigned();
	void notifyPartitionUnassigned();
	void addBounds();
	void removeBounds();

protected:
	BoundingVolumeBase* m_bounds;
	BoundingVolumeBase* m_worldBounds;
	bool m_boundsInvalid;
	PickingCollisionVO m_pickingCollisionVO;
	float m_zOffset;

private:
	bool m_showBounds;
	bool m_boundsIsShown;
	bool m_worldBoundsInvalid;
	EntityNode* m_partitionNode;
	ControllerBase* m_controller;
};

AWAY_NAMESPACE_END