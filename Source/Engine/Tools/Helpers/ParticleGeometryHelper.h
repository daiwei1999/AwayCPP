#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ParticleGeometryHelper
{
public:
	static ParticleGeometry* generateGeometry(std::vector<Geometry*>& geometries, ParticleGeometryTransform* transforms = nullptr);
};

AWAY_NAMESPACE_END