#pragma once

#include "Common.h"
#include "LightPickerBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * StaticLightPicker is a light picker that provides a static set of lights. The lights can be reassigned, but
 * if the configuration changes (number of directional lights, point lights, etc), a material recompilation may
 * occur.
 */
class StaticLightPicker : public LightPickerBase
{
	DEFINE_CALLBACK_CLASS(StaticLightPicker, onCastShadowChange)

public:
	StaticLightPicker(LightVector& lights);

	LightVector& getLights() { return m_lights; }
	void setLights(LightVector& value);

private:
	void onCastShadowChange(Event* event);
	void updateDirectionalCasting(DirectionalLight* light);
	void updatePointCasting(PointLight* light);

private:
	LightVector m_lights;
};

AWAY_NAMESPACE_END