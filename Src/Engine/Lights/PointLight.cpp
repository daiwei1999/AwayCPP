#include "PointLight.h"
#include "CubeMapShadowMapper.h"
#include "PointLightNode.h"
#include "BoundingSphere.h"
#include "IRenderable.h"

USING_AWAY_NAMESPACE

PointLight::PointLight()
{
	m_bounds = new BoundingSphere();
	m_worldBounds = new BoundingSphere();
	m_type = LightType::Point;
	m_radius = 90000;
	m_fallOff = 100000;
	m_fallOffFactor = 1 / (m_fallOff * m_fallOff - m_radius * m_radius);
}

void PointLight::setRadius(float value)
{
	m_radius = value;
	if (m_radius < 0)
	{
		m_radius = 0;
	}
	else if (m_radius > m_fallOff)
	{
		m_fallOff = m_radius;
		invalidateBounds();
	}

	m_fallOffFactor = 1 / (m_fallOff * m_fallOff - m_radius * m_radius);
}

void PointLight::setFallOff(float value)
{
	m_fallOff = value;
	if (m_fallOff < 0)
		m_fallOff = 0;
	if (m_fallOff < m_radius)
		m_radius = m_fallOff;

	m_fallOffFactor = 1 / (m_fallOff * m_fallOff - m_radius * m_radius);
	invalidateBounds();
}

void PointLight::getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result)
{
	// todo: do not use lookAt on Light
	Matrix3D matrix;
	matrix.copyFrom(renderable->getRenderSceneTransform(camera));
	matrix.append(m_parent->getInverseSceneTransform());

	Vector3D pos;
	matrix.copyColumnTo(3, pos);
	lookAt(pos);

	matrix.copyFrom(renderable->getRenderSceneTransform(camera));
	matrix.append(getInverseSceneTransform());
	matrix.copyColumnTo(3, pos);

	Vector3D v1, v2;
	BoundingVolumeBase* bounds = renderable->getSourceEntity()->getBounds();
	Matrix3D::deltaTransformVector(matrix, bounds->getMin(), &v1);
	float d1 = v1.getLengthSquared();
	Matrix3D::deltaTransformVector(matrix, bounds->getMax(), &v2);
	float d2 = v2.getLengthSquared();
	float d = std::sqrt(d1 > d2 ? d1 : d2);
	float zMin = pos.m_z - d;
	float zMax = pos.m_z + d;

	float(&raw)[16] = result.m_rawData;
	raw[5] = raw[0] = zMin / d;
	raw[10] = zMax / (zMax - zMin);
	raw[11] = 1;
	raw[1] = raw[2] = raw[3] = raw[4] = raw[6] = raw[7] = raw[8] = raw[9] = raw[12] = raw[13] = raw[15] = 0;
	raw[14] = -zMin * raw[10];
	result.prepend(matrix);
}

void PointLight::updateBounds()
{
	Vector3D center;
	m_bounds->fromSphere(center, m_fallOff);
	m_boundsInvalid = false;
}

ShadowMapperBase* PointLight::createShadowMapper()
{
	return new CubeMapShadowMapper();
}

EntityNode* PointLight::createEntityPartitionNode()
{
	return new PointLightNode(this);
}