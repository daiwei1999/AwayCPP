#include "StaticLightPicker.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "LightEvent.h"

USING_AWAY_NAMESPACE

StaticLightPicker::StaticLightPicker(LightVector& lights)
{
	setLights(lights);
}

void StaticLightPicker::setLights(LightVector& value)
{
	for (auto light : m_lights)
		light->removeEventListener(LightEvent::CASTS_SHADOW_CHANGE, CALLBACK_OBJECT(onCastShadowChange));

	m_lights = value;

	int numPointLights = 0;
	int numDirectionalLights = 0;
	int numCastingPointLights = 0;
	int numCastingDirectionalLights = 0;
	int numLightProbes = 0;
	for (auto light : m_lights)
	{
		light->addEventListener(LightEvent::CASTS_SHADOW_CHANGE, CALLBACK_OBJECT(onCastShadowChange));

		switch (light->getType())
		{
		case LightType::Point:
			if (light->getCastsShadows())
			{
				m_castingPointLights.push_back((PointLight*)light);
				numCastingPointLights++;
			}
			else
			{
				m_pointLights.push_back((PointLight*)light);
				numPointLights++;
			}
			break;
		case LightType::Directional:
			if (light->getCastsShadows())
			{
				m_castingDirectionalLights.push_back((DirectionalLight*)light);
				numCastingDirectionalLights++;
			}
			else
			{
				m_directionalLights.push_back((DirectionalLight*)light);
				numDirectionalLights++;
			}
			break;
		case LightType::Probe:
			m_lightProbes.push_back((LightProbe*)light);
			numLightProbes++;
			break;
		}
	}

	if (m_numDirectionalLights == numDirectionalLights && m_numPointLights == numPointLights && m_numLightProbes == numLightProbes &&
		m_numCastingPointLights == numCastingPointLights && m_numCastingDirectionalLights == numCastingDirectionalLights)
		return;
	
	m_numPointLights = numPointLights;
	m_numDirectionalLights = numDirectionalLights;
	m_numCastingPointLights = numCastingPointLights;
	m_numCastingDirectionalLights = numCastingDirectionalLights;
	m_numLightProbes = numLightProbes;

	m_lightProbeWeights.resize((int)std::ceil(numLightProbes / 4.f) * 4);

	// notify material lights have changed
	Event event(Event::CHANGE);
	dispatchEvent(&event);
}

void StaticLightPicker::onCastShadowChange(Event* event)
{
	LightBase* light = ((LightEvent*)event)->getLight();
	switch (light->getType())
	{
	case LightType::Point:
		updatePointCasting((PointLight*)light);
		break;
	case LightType::Directional:
		updateDirectionalCasting((DirectionalLight*)light);
		break;
	}

	Event e(Event::CHANGE);
	dispatchEvent(&e);
}

void StaticLightPicker::updateDirectionalCasting(DirectionalLight* light)
{
	if (light->getCastsShadows())
	{
		m_numDirectionalLights--;
		m_numCastingDirectionalLights++;
		m_directionalLights.erase(std::find(m_directionalLights.begin(), m_directionalLights.end(), light));
		m_castingDirectionalLights.push_back(light);
	}
	else
	{
		m_numDirectionalLights++;
		m_numCastingDirectionalLights--;
		m_castingDirectionalLights.erase(std::find(m_castingDirectionalLights.begin(), m_castingDirectionalLights.end(), light));
		m_directionalLights.push_back(light);
	}
}

void StaticLightPicker::updatePointCasting(PointLight* light)
{
	if (light->getCastsShadows())
	{
		m_numPointLights--;
		m_numCastingPointLights++;
		m_pointLights.erase(std::find(m_pointLights.begin(), m_pointLights.end(), light));
		m_castingPointLights.push_back(light);
	}
	else
	{
		m_numPointLights++;
		m_numCastingPointLights--;
		m_castingPointLights.erase(std::find(m_castingPointLights.begin(), m_castingPointLights.end(), light));
		m_pointLights.push_back(light);
	}
}