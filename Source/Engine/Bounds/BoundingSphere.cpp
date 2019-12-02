#include "BoundingSphere.h"
#include "WireframeSphere.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

BoundingSphere::BoundingSphere()
{
	m_radius = 0;
	m_centerX = m_centerY = m_centerZ = 0;
}

void BoundingSphere::nullify()
{
	BoundingVolumeBase::nullify();

	m_radius = 0;
	m_centerX = m_centerY = m_centerZ = 0;
}

bool BoundingSphere::isInFrustum(PlaneVector* planes, int numPlanes)
{
	for (int i = 0; i < numPlanes; i++)
	{
		Plane3D& plane = (*planes)[i];
		float flippedExtentX = plane.m_a < 0 ? -m_radius : m_radius;
		float flippedExtentY = plane.m_b < 0 ? -m_radius : m_radius;
		float flippedExtentZ = plane.m_c < 0 ? -m_radius : m_radius;
		if (plane.m_a * (m_centerX + flippedExtentX) + plane.m_b * (m_centerY + flippedExtentY) + plane.m_c * (m_centerZ + flippedExtentZ) - plane.m_d < 0)
			return false;
	}
	return true;
}

void BoundingSphere::fromSphere(Vector3D& center, float radius)
{
	m_centerX = center.m_x;
	m_centerY = center.m_y;
	m_centerZ = center.m_z;
	m_radius = radius;
	m_max.m_x = m_centerX + radius;
	m_max.m_y = m_centerY + radius;
	m_max.m_z = m_centerZ + radius;
	m_min.m_x = m_centerX - radius;
	m_min.m_y = m_centerY - radius;
	m_min.m_z = m_centerZ - radius;
	m_aabbPointsDirty = true;
	if (m_boundingRenderable)
		updateBoundingRenderable();
}

void BoundingSphere::fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	m_centerX = (maxX + minX) * .5f;
	m_centerY = (maxY + minY) * .5f;
	m_centerZ = (maxZ + minZ) * .5f;

	float d = maxX - minX;
	float y = maxY - minY;
	float z = maxZ - minZ;
	if (y > d)
		d = y;

	if (z > d)
		d = z;

	m_radius = d * std::sqrt(.5f);
	BoundingVolumeBase::fromExtremes(minX, minY, minZ, maxX, maxY, maxZ);
}

float BoundingSphere::rayIntersection(Vector3D& position, Vector3D& direction, Vector3D& targetNormal)
{
	if (containsPoint(position))
		return 0;

	float px = position.m_x - m_centerX, py = position.m_y - m_centerY, pz = position.m_z - m_centerZ;
	float vx = direction.m_x, vy = direction.m_y, vz = direction.m_z;
	float a = vx * vx + vy * vy + vz * vz;
	float b = 2 * (px * vx + py * vy + pz * vz);
	float c = px * px + py * py + pz * pz - m_radius * m_radius;
	float det = b * b - 4 * a * c;
	if (det >= 0)
	{
		// ray goes through sphere
		float sqrtDet = std::sqrt(det);
		float rayEntryDistance = (-b - sqrtDet) / (2 * a);
		if (rayEntryDistance >= 0)
		{
			targetNormal.m_x = px + rayEntryDistance * vx;
			targetNormal.m_y = py + rayEntryDistance * vy;
			targetNormal.m_z = pz + rayEntryDistance * vz;
			targetNormal.normalize();
			return rayEntryDistance;
		}
	}

	return -1;
}

bool BoundingSphere::containsPoint(Vector3D& position)
{
	float px = position.m_x - m_centerX, py = position.m_y - m_centerY, pz = position.m_z - m_centerZ;
	float distance = std::sqrt(px * px + py * py + pz * pz);
	return distance <= m_radius;
}

void BoundingSphere::copyFrom(BoundingVolumeBase* bounds)
{
	BoundingSphere* sphereBounds = (BoundingSphere*)bounds;
	m_radius = sphereBounds->m_radius;
	m_centerX = sphereBounds->m_centerX;
	m_centerY = sphereBounds->m_centerY;
	m_centerZ = sphereBounds->m_centerZ;
	BoundingVolumeBase::copyFrom(bounds);
}

void BoundingSphere::transformFrom(BoundingVolumeBase* bounds, Matrix3D& matrix)
{
	BoundingSphere* sphere = (BoundingSphere*)bounds;
	float cx = sphere->m_centerX;
	float cy = sphere->m_centerY;
	float cz = sphere->m_centerZ;

	const float* raw = matrix.m_rawData;
	float m11 = raw[0], m12 = raw[4], m13 = raw[8], m14 = raw[12];
	float m21 = raw[1], m22 = raw[5], m23 = raw[9], m24 = raw[13];
	float m31 = raw[2], m32 = raw[6], m33 = raw[10], m34 = raw[14];

	m_centerX = cx * m11 + cy * m12 + cz * m13 + m14;
	m_centerY = cx * m21 + cy * m22 + cz * m23 + m24;
	m_centerZ = cx * m31 + cy * m32 + cz * m33 + m34;

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
	float r = sphere->m_radius;
	float rx = m11 + m12 + m13;
	float ry = m21 + m22 + m23;
	float rz = m31 + m32 + m33;
	m_radius = r * std::sqrt(rx * rx + ry * ry + rz * rz);

	m_min.m_x = m_centerX - m_radius;
	m_min.m_y = m_centerY - m_radius;
	m_min.m_z = m_centerZ - m_radius;
	m_max.m_x = m_centerX + m_radius;
	m_max.m_y = m_centerY + m_radius;
	m_max.m_z = m_centerZ + m_radius;
}

void BoundingSphere::updateBoundingRenderable()
{
	float sc = m_radius;
	if (sc == 0)
		sc = 0.001f;
	m_boundingRenderable->setScaleX(sc);
	m_boundingRenderable->setScaleY(sc);
	m_boundingRenderable->setScaleZ(sc);
	m_boundingRenderable->setX(m_centerX);
	m_boundingRenderable->setY(m_centerY);
	m_boundingRenderable->setZ(m_centerZ);
}

WireframePrimitiveBase* BoundingSphere::createBoundingRenderable()
{
	return new WireframeSphere(1, 16, 12, 0xffffff);
}