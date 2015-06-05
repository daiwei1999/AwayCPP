#include "ConeGeometry.h"

USING_AWAY_NAMESPACE

ConeGeometry::ConeGeometry(float radius, float height, unsigned short segmentsW, unsigned short segmentsH, bool closed, bool yUp) : CylinderGeometry(0, radius, height, segmentsW, segmentsH, false, closed, true, yUp)
{

}

void ConeGeometry::setRadius(float value)
{
	if (value != m_bottomRadius)
	{
		m_bottomRadius = value;
		invalidateGeometry();
	}
}