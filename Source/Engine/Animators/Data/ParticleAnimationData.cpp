#include "ParticleAnimationData.h"

USING_AWAY_NAMESPACE

ParticleAnimationData::ParticleAnimationData(int index, float startTime, float duration, float delay, ParticleData* particle)
{
	m_index = index;
	m_startTime = startTime;
	m_totalTime = duration + delay;
	m_duration = duration;
	m_delay = delay;
	m_startVertexIndex = particle->m_startVertexIndex;
	m_numVertices = particle->m_numVertices;
}