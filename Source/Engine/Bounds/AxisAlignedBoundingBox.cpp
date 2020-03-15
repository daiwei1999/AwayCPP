#include "AxisAlignedBoundingBox.h"
#include "WireframeCube.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
{
	m_centerX = m_centerY = m_centerZ = 0;
	m_halfExtentsX = m_halfExtentsY = m_halfExtentsZ = 0;
}

void AxisAlignedBoundingBox::nullify()
{
	BoundingVolumeBase::nullify();

	m_centerX = m_centerY = m_centerZ = 0;
	m_halfExtentsX = m_halfExtentsY = m_halfExtentsZ = 0;
}

bool AxisAlignedBoundingBox::isInFrustum(PlaneVector* planes, int numPlanes)
{
	for (int i = 0; i < numPlanes; i++)
	{
		Plane3D& plane = (*planes)[i];
		float flippedExtentX = plane.m_a < 0 ? -m_halfExtentsX : m_halfExtentsX;
		float flippedExtentY = plane.m_b < 0 ? -m_halfExtentsY : m_halfExtentsY;
		float flippedExtentZ = plane.m_c < 0 ? -m_halfExtentsZ : m_halfExtentsZ;
		if (plane.m_a * (m_centerX + flippedExtentX) + plane.m_b * (m_centerY + flippedExtentY) + plane.m_c * (m_centerZ + flippedExtentZ) - plane.m_d < 0)
			return false;
	}
	return true;
}

float AxisAlignedBoundingBox::rayIntersection(Vector3D& position, Vector3D& direction, Vector3D& targetNormal)
{
	if (containsPoint(position))
		return 0;

	float px = position.m_x - m_centerX, py = position.m_y - m_centerY, pz = position.m_z - m_centerZ;
	float vx = direction.m_x, vy = direction.m_y, vz = direction.m_z;
	float ix, iy, iz;
	float rayEntryDistance;

	// ray-plane tests
	if (vx < 0)
	{
		rayEntryDistance = (m_halfExtentsX - px) / vx;
		if (rayEntryDistance > 0)
		{
			iy = py + rayEntryDistance * vy;
			iz = pz + rayEntryDistance * vz;
			if (iy > -m_halfExtentsY && iy < m_halfExtentsY && iz > -m_halfExtentsZ && iz < m_halfExtentsZ)
			{
				targetNormal.m_x = 1;
				targetNormal.m_y = 0;
				targetNormal.m_z = 0;
				return rayEntryDistance;
			}
		}
	}
	if (vx > 0)
	{
		rayEntryDistance = (-m_halfExtentsX - px) / vx;
		if (rayEntryDistance > 0)
		{
			iy = py + rayEntryDistance * vy;
			iz = pz + rayEntryDistance * vz;
			if (iy > -m_halfExtentsY && iy < m_halfExtentsY && iz > -m_halfExtentsZ && iz < m_halfExtentsZ)
			{
				targetNormal.m_x = -1;
				targetNormal.m_y = 0;
				targetNormal.m_z = 0;
				return rayEntryDistance;
			}
		}
	}
	if (vy < 0)
	{
		rayEntryDistance = (m_halfExtentsY - py) / vy;
		if (rayEntryDistance > 0)
		{
			ix = px + rayEntryDistance * vx;
			iz = pz + rayEntryDistance * vz;
			if (ix > -m_halfExtentsX && ix < m_halfExtentsX && iz > -m_halfExtentsZ && iz < m_halfExtentsZ)
			{
				targetNormal.m_x = 0;
				targetNormal.m_y = 1;
				targetNormal.m_z = 0;
				return rayEntryDistance;
			}
		}
	}
	if (vy > 0)
	{
		rayEntryDistance = (-m_halfExtentsY - py) / vy;
		if (rayEntryDistance > 0)
		{
			ix = px + rayEntryDistance * vx;
			iz = pz + rayEntryDistance * vz;
			if (ix > -m_halfExtentsX && ix < m_halfExtentsX && iz > -m_halfExtentsZ && iz < m_halfExtentsZ)
			{
				targetNormal.m_x = 0;
				targetNormal.m_y = -1;
				targetNormal.m_z = 0;
				return rayEntryDistance;
			}
		}
	}
	if (vz < 0)
	{
		rayEntryDistance = (m_halfExtentsZ - pz) / vz;
		if (rayEntryDistance > 0)
		{
			ix = px + rayEntryDistance * vx;
			iy = py + rayEntryDistance * vy;
			if (iy > -m_halfExtentsY && iy < m_halfExtentsY && ix > -m_halfExtentsX && ix < m_halfExtentsX)
			{
				targetNormal.m_x = 0;
				targetNormal.m_y = 0;
				targetNormal.m_z = 1;
				return rayEntryDistance;
			}
		}
	}
	if (vz > 0)
	{
		rayEntryDistance = (-m_halfExtentsZ - pz) / vz;
		if (rayEntryDistance > 0)
		{
			ix = px + rayEntryDistance * vx;
			iy = py + rayEntryDistance * vy;
			if (iy > -m_halfExtentsY && iy < m_halfExtentsY && ix > -m_halfExtentsX && ix < m_halfExtentsX)
			{
				targetNormal.m_x = 0;
				targetNormal.m_y = 0;
				targetNormal.m_z = -1;
				return rayEntryDistance;
			}
		}
	}

	return -1;
}

bool AxisAlignedBoundingBox::containsPoint(Vector3D& position)
{
	float px = position.m_x - m_centerX, py = position.m_y - m_centerY, pz = position.m_z - m_centerZ;
	return px <= m_halfExtentsX && px >= -m_halfExtentsX && py <= m_halfExtentsY && py >= -m_halfExtentsY && pz <= m_halfExtentsZ && pz >= -m_halfExtentsZ;
}

void AxisAlignedBoundingBox::fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	m_centerX = (maxX + minX) * .5f;
	m_centerY = (maxY + minY) * .5f;
	m_centerZ = (maxZ + minZ) * .5f;
	m_halfExtentsX = (maxX - minX) * .5f;
	m_halfExtentsY = (maxY - minY) * .5f;
	m_halfExtentsZ = (maxZ - minZ) * .5f;
	BoundingVolumeBase::fromExtremes(minX, minY, minZ, maxX, maxY, maxZ);
}

void AxisAlignedBoundingBox::updateBoundingRenderable()
{
	m_boundingRenderable->setScaleX(std::max(m_halfExtentsX * 2, 0.001f));
	m_boundingRenderable->setScaleY(std::max(m_halfExtentsY * 2, 0.001f));
	m_boundingRenderable->setScaleZ(std::max(m_halfExtentsZ * 2, 0.001f));
	m_boundingRenderable->setX(m_centerX);
	m_boundingRenderable->setY(m_centerY);
	m_boundingRenderable->setZ(m_centerZ);
}

WireframePrimitiveBase* AxisAlignedBoundingBox::createBoundingRenderable()
{
	return new WireframeCube(1, 1, 1, 0xffffff);
}

void AxisAlignedBoundingBox::copyFrom(BoundingVolumeBase* bounds)
{
	AxisAlignedBoundingBox* aabbBounds = (AxisAlignedBoundingBox*)bounds;
	m_centerX = aabbBounds->m_centerX;
	m_centerY = aabbBounds->m_centerY;
	m_centerZ = aabbBounds->m_centerZ;
	m_halfExtentsX = aabbBounds->m_halfExtentsX;
	m_halfExtentsY = aabbBounds->m_halfExtentsY;
	m_halfExtentsZ = aabbBounds->m_halfExtentsZ;
	BoundingVolumeBase::copyFrom(bounds);
}

void AxisAlignedBoundingBox::transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix)
{
	AxisAlignedBoundingBox* aabb = static_cast<AxisAlignedBoundingBox*>(bounds);
	float cx = aabb->m_centerX;
	float cy = aabb->m_centerY;
	float cz = aabb->m_centerZ;
	const float* raw = matrix.m_rawData;
	float m11 = raw[0];
	float m12 = raw[4];
	float m13 = raw[8];
	float m21 = raw[1];
	float m22 = raw[5];
	float m23 = raw[9];
	float m31 = raw[2];
	float m32 = raw[6];
	float m33 = raw[10];

	m_centerX = cx * m11 + cy * m12 + cz * m13 + raw[12];
	m_centerY = cx * m21 + cy * m22 + cz * m23 + raw[13];
	m_centerZ = cx * m31 + cy * m32 + cz * m33 + raw[14];

	if (m11 < 0)
		m11 = -m11;
	if (m12 < 0)
		m12 = -m12;
	if (m13 < 0)
		m13 = -m13;
	if (m21 < 0)
		m21 = -m21;
	if (m22 < 0)
		m22 = -m22;
	if (m23 < 0)
		m23 = -m23;
	if (m31 < 0)
		m31 = -m31;
	if (m32 < 0)
		m32 = -m32;
	if (m33 < 0)
		m33 = -m33;

	float hx = aabb->m_halfExtentsX;
	float hy = aabb->m_halfExtentsY;
	float hz = aabb->m_halfExtentsZ;
	m_halfExtentsX = hx * m11 + hy * m12 + hz * m13;
	m_halfExtentsY = hx * m21 + hy * m22 + hz * m23;
	m_halfExtentsZ = hx * m31 + hy * m32 + hz * m33;

	m_min.m_x = m_centerX - m_halfExtentsX;
	m_min.m_y = m_centerY - m_halfExtentsY;
	m_min.m_z = m_centerZ - m_halfExtentsZ;
	m_max.m_x = m_centerX + m_halfExtentsX;
	m_max.m_y = m_centerY + m_halfExtentsY;
	m_max.m_z = m_centerZ + m_halfExtentsZ;

	m_aabbPointsDirty = true;
}
