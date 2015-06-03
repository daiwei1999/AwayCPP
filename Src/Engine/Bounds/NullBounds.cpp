#include "NullBounds.h"
#include "MathConsts.h"
#include "WireframeSphere.h"

USING_AWAY_NAMESPACE

NullBounds::NullBounds(bool alwaysIn, WireframePrimitiveBase* renderable)
{
	m_alwaysIn = alwaysIn;
	m_renderable = renderable;
	m_max.m_x = m_max.m_y = m_max.m_z = MathConsts::Infinity;
	m_min.m_x = m_min.m_y = m_min.m_z = m_alwaysIn ? -MathConsts::Infinity : MathConsts::Infinity;
}

void NullBounds::fromGeometry(Geometry* geometry)
{

}

void NullBounds::fromSphere(Vector3D& center, float radius)
{

}

void NullBounds::fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{

}

bool NullBounds::isInFrustum(PlaneVector* planes, int numPlanes)
{
	return m_alwaysIn;
}

void NullBounds::copyFrom(BoundingVolumeBase* bounds)
{
	m_alwaysIn = ((NullBounds*)bounds)->m_alwaysIn;
}

void NullBounds::transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix)
{
	m_alwaysIn = ((NullBounds*)bounds)->m_alwaysIn;
}

void NullBounds::updateBoundingRenderable()
{

}

WireframePrimitiveBase* NullBounds::createBoundingRenderable()
{
	return m_renderable ? m_renderable : new WireframeSphere(100, 16, 12, 0xffffff);
}
