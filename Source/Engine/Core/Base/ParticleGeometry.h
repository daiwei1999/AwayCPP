#pragma once

#include "Common.h"
#include "Geometry.h"

AWAY_NAMESPACE_BEGIN

class ParticleGeometry : public Geometry
{
public:
	std::vector<ParticleData*> m_particles;
	int m_numParticles;
};

AWAY_NAMESPACE_END