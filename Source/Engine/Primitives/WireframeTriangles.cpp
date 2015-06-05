#include "WireframeTriangles.h"

USING_AWAY_NAMESPACE

WireframeTriangles::WireframeTriangles(unsigned int color) : WireframePrimitiveBase(color)
{

}

void WireframeTriangles::buildGeometry()
{
	int index = 0, size = m_points.size();
	for (int i = 0; i < size; i += 3)
	{
		Vector3D& v0 = m_points[i];
		Vector3D& v1 = m_points[i + 1];
		Vector3D& v2 = m_points[i + 2];
		updateOrAddSegment(index++, v0, v1);
		updateOrAddSegment(index++, v1, v2);
		updateOrAddSegment(index++, v2, v0);
	}
}