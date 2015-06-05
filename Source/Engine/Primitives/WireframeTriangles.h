#pragma once

#include "Common.h"
#include "WireframePrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class WireframeTriangles : public WireframePrimitiveBase
{
public:
	WireframeTriangles(unsigned int color = 0xFFFFFF);

	void addPoint(Vector3D& point) { m_points.push_back(point); }

protected:
	void buildGeometry() override;

private:
	std::vector<Vector3D> m_points;
};

AWAY_NAMESPACE_END