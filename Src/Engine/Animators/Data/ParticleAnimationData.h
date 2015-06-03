#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ParticleData
{
public:
	int m_particleIndex;
	int m_numVertices;
	int m_startVertexIndex;
	ISubGeometry* m_subGeometry;
};

class ParticleAnimationData
{
public:
	ParticleAnimationData(int index, float startTime, float duration, float delay, ParticleData* particle);

public:
	int m_index;
	float m_startTime;
	float m_totalTime;
	float m_duration;
	float m_delay;
	int m_startVertexIndex;
	int m_numVertices;
};

AWAY_NAMESPACE_END