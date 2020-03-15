#include "BoundingVolumeBase.h"
#include "Geometry.h"
#include "ISubGeometry.h"
#include "WireframePrimitiveBase.h"

USING_AWAY_NAMESPACE

BoundingVolumeBase::BoundingVolumeBase()
{
	m_aabbPointsDirty = true;
	m_boundingRenderable = nullptr;
}

float* BoundingVolumeBase::getAABBPoints()
{
	if (m_aabbPointsDirty)
		updateAABBPoints();

	return m_aabbPoints;
}

WireframePrimitiveBase* BoundingVolumeBase::getBoundingRenderable()
{
	if (!m_boundingRenderable)
	{
		m_boundingRenderable = createBoundingRenderable();
		updateBoundingRenderable();
	}
	return m_boundingRenderable;
}

void BoundingVolumeBase::nullify()
{
	m_min.m_x = m_min.m_y = m_min.m_z = 0;
	m_max.m_x = m_max.m_y = m_max.m_z = 0;
	m_aabbPointsDirty = true;
	if (m_boundingRenderable)
		updateBoundingRenderable();
}

void BoundingVolumeBase::disposeRenderable()
{
	if (m_boundingRenderable)
	{
		delete m_boundingRenderable;
		m_boundingRenderable = nullptr;
	}
}

void BoundingVolumeBase::fromGeometry(Geometry* geometry)
{
	SubGeometryVector& subGeoms = geometry->getSubGeometries();
	size_t numSubGeoms = subGeoms.size();
	if (numSubGeoms > 0)
	{
		ISubGeometry* subGeom = subGeoms[0];
		float* vertices = subGeom->getVertexData();
		int i = subGeom->getVertexOffset();

		float minX, minY, minZ, maxX, maxY, maxZ, v;
		minX = maxX = vertices[i];
		minY = maxY = vertices[i + 1];
		minZ = maxZ = vertices[i + 2];

		int stride, vertexDataLen;
		for (size_t j = 0; j < numSubGeoms; ++j)
		{
			subGeom = subGeoms[j];
			vertices = subGeom->getVertexData();
			i = subGeom->getVertexOffset();
			stride = subGeom->getVertexStride();
			vertexDataLen = subGeom->getVertexCount() * stride;

			while (i < vertexDataLen)
			{
				v = vertices[i];
				if (v < minX)
					minX = v;
				else if (v > maxX)
					maxX = v;

				v = vertices[i + 1];
				if (v < minY)
					minY = v;
				else if (v > maxY)
					maxY = v;

				v = vertices[i + 2];
				if (v < minZ)
					minZ = v;
				else if (v > maxZ)
					maxZ = v;

				i += stride;
			}
		}

		fromExtremes(minX, minY, minZ, maxX, maxY, maxZ);
	}
	else
		fromExtremes(0, 0, 0, 0, 0, 0);
}

void BoundingVolumeBase::fromSkeletonPose(SkeletonPose* skeletonPose)
{

}

void BoundingVolumeBase::fromSphere(Vector3D& center, float radius)
{
	// this is BETTER overridden, because most volumes will have shortcuts for this
	// but then again, sphere already overrides it, and if we'd call "fromSphere", it'd probably need a sphere bound anyway
	fromExtremes(center.m_x - radius, center.m_y - radius, center.m_z - radius, center.m_x + radius, center.m_y + radius, center.m_z + radius);
}

void BoundingVolumeBase::fromExtremes(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
	m_min.m_x = minX;
	m_min.m_y = minY;
	m_min.m_z = minZ;
	m_max.m_x = maxX;
	m_max.m_y = maxY;
	m_max.m_z = maxZ;
	m_aabbPointsDirty = true;
	if (m_boundingRenderable)
		updateBoundingRenderable();
}

bool BoundingVolumeBase::overlaps(BoundingVolumeBase* bounds)
{
	Vector3D& min = bounds->m_min;
	Vector3D& max = bounds->m_max;
	return m_max.m_x > min.m_x && m_min.m_x < max.m_x && m_max.m_y > min.m_y && m_min.m_y < max.m_y && m_max.m_z > min.m_z && m_min.m_z < max.m_z;
}

float BoundingVolumeBase::rayIntersection(Vector3D& position, Vector3D& direction, Vector3D& targetNormal)
{
	return -1;
}

bool BoundingVolumeBase::containsPoint(Vector3D& position)
{
	return false;
}

void BoundingVolumeBase::merge(BoundingVolumeBase* bounds)
{
	Vector3D& min = bounds->m_min;
	Vector3D& max = bounds->m_max;
	fromExtremes(std::min(m_min.m_x, min.m_x), std::min(m_min.m_y, min.m_y), std::min(m_min.m_z, min.m_z),
		std::max(m_max.m_x, max.m_x), std::max(m_max.m_y, max.m_y), std::max(m_max.m_z, max.m_z));
}

void BoundingVolumeBase::copyFrom(BoundingVolumeBase* bounds)
{
	m_min.copyFrom(bounds->m_min);
	m_max.copyFrom(bounds->m_max);
}

void BoundingVolumeBase::updateAABBPoints()
{
	m_aabbPoints[0] = m_aabbPoints[6] = m_aabbPoints[12] = m_aabbPoints[18] = m_min.m_x;
	m_aabbPoints[1] = m_aabbPoints[4] = m_aabbPoints[13] = m_aabbPoints[16] = m_min.m_y;
	m_aabbPoints[2] = m_aabbPoints[5] = m_aabbPoints[8] = m_aabbPoints[11] = m_min.m_z;
	m_aabbPoints[3] = m_aabbPoints[9] = m_aabbPoints[15] = m_aabbPoints[21] = m_max.m_x;
	m_aabbPoints[7] = m_aabbPoints[10] = m_aabbPoints[19] = m_aabbPoints[22] = m_max.m_y;
	m_aabbPoints[14] = m_aabbPoints[17] = m_aabbPoints[20] = m_aabbPoints[23] = m_max.m_z;
	m_aabbPointsDirty = false;
}
