#pragma once

#include "Common.h"
#include "LightBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * PointLight represents an omni-directional light. The light is emitted from a given position in the scene.
 */
class PointLight : public LightBase
{
public:
	PointLight();

	float getRadius() { return m_radius; }
	void setRadius(float value);
	float getFallOff() { return m_fallOff; }
	void setFallOff(float value);
	
	float getFallOffFactor() { return m_fallOffFactor; }
	void getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result) override;

protected:
	void updateBounds() override;
	ShadowMapperBase* createShadowMapper() override;
	EntityNode* createEntityPartitionNode() override;

private:
	float m_radius;
	float m_fallOff;
	float m_fallOffFactor;
};

AWAY_NAMESPACE_END