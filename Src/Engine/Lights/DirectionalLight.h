#pragma once

#include "Common.h"
#include "LightBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * DirectionalLight represents an idealized light "at infinity", to be used for distant light sources such as the sun.
 * In any position in the scene, the light raytracing will always be parallel.
 * Although the position of the light does not impact its effect, it can be used along with lookAt to intuitively
 * create day cycles by orbiting the position around a center point and using lookAt at that position.
 */
class DirectionalLight : public LightBase
{
public:
	DirectionalLight(float xDir = 0, float yDir = -1, float zDir = 1);

	Vector3D* getSceneDirection();
	Vector3D* getDirection() { return &m_direction; }
	void setDirection(Vector3D& value);

	void getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result) override;

protected:
	void updateBounds() override;
	void updateSceneTransform() override;

	EntityNode* createEntityPartitionNode() override;
	ShadowMapperBase* createShadowMapper() override;

private:
	Vector3D m_direction;
	Vector3D m_sceneDirection;
};

AWAY_NAMESPACE_END