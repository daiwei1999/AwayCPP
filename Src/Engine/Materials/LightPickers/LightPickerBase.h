#pragma once

#include "Common.h"
#include "EventDispatcher.h"

AWAY_NAMESPACE_BEGIN

/**
 * LightPickerBase provides an abstract base class for light picker classes. These classes are responsible for
 * feeding materials with relevant lights. Usually, StaticLightPicker can be used, but LightPickerBase can be
 * extended to provide more application-specific dynamic selection of lights.
 *
 * @see StaticLightPicker
 */
class LightPickerBase : public EventDispatcher
{
public:
	LightPickerBase();

	int getPointLightCount() { return m_numPointLights; }
	int getDirectionalLightCount() { return m_numDirectionalLights; }
	int getCastingPointLightCount() { return m_numCastingPointLights; }
	int getCastingDirectionalLightCount() { return m_numCastingDirectionalLights; }
	int getLightProbeCount() { return m_numLightProbes; }
	
	PointLightVector& getPointLights() { return m_pointLights; }
	DirectionalLightVector& getDirectionalLights() { return m_directionalLights; }
	PointLightVector& getCastingPointLights() { return m_castingPointLights; }
	DirectionalLightVector& getCastingDirectionalLights() { return m_castingDirectionalLights; }
	LightProbeVector& getLightProbes() { return m_lightProbes; }
	WeightVector& getLightProbeWeights() { return m_lightProbeWeights; }

	void collectLights(IRenderable* renderable, EntityCollector* entityCollector);

private:
	void updateProbeWeights(IRenderable* renderable);

protected:
	int m_numPointLights;
	int m_numDirectionalLights;
	int m_numCastingPointLights;
	int m_numCastingDirectionalLights;
	int m_numLightProbes;
	PointLightVector m_pointLights;
	PointLightVector m_castingPointLights;
	DirectionalLightVector m_directionalLights;
	DirectionalLightVector m_castingDirectionalLights;
	LightProbeVector m_lightProbes;
	WeightVector m_lightProbeWeights;
};

AWAY_NAMESPACE_END