#include "LightPickerBase.h"
#include "IRenderable.h"
#include "LightProbe.h"

USING_AWAY_NAMESPACE

LightPickerBase::LightPickerBase()
{
	m_numPointLights = 0;
	m_numDirectionalLights = 0;
	m_numCastingPointLights = 0;
	m_numCastingDirectionalLights = 0;
	m_numLightProbes = 0;
}

void LightPickerBase::collectLights(IRenderable* renderable, EntityCollector* entityCollector)
{
	if (m_numLightProbes > 0)
		updateProbeWeights(renderable);
}

void LightPickerBase::updateProbeWeights(IRenderable* renderable)
{
	Vector3D* objectPos = renderable->getSourceEntity()->getScenePosition();
	float rx = objectPos->m_x, ry = objectPos->m_y, rz = objectPos->m_z;
	float total = 0;

	// calculates weights for probes
	for (int i = 0; i < m_numLightProbes; i++)
	{
		Vector3D* lightPos = m_lightProbes[i]->getScenePosition();
		float dx = rx - lightPos->m_x;
		float dy = ry - lightPos->m_y;
		float dz = rz - lightPos->m_z;
		// weight is inversely proportional to square of distance
		float w = dx * dx + dy * dy + dz * dz;

		// just... huge if at the same spot
		w = (w > .00001f) ? (1 / w) : 50000000;
		m_lightProbeWeights[i] = w;
		total += w;
	}

	// normalize
	total = 1 / total;
	for (int i = 0; i < m_numLightProbes; i++)
		m_lightProbeWeights[i] *= total;
}