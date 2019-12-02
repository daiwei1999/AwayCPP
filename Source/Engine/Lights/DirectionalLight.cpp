#include "DirectionalLight.h"
#include "NullBounds.h"
#include "DirectionalLightNode.h"
#include "DirectionalShadowMapper.h"
#include "IRenderable.h"

USING_AWAY_NAMESPACE

DirectionalLight::DirectionalLight(float xDir, float yDir, float zDir)
{
	m_bounds = new NullBounds();
	m_worldBounds = new NullBounds();
	m_type = LightType::Directional;
	Vector3D dir(xDir, yDir, zDir);
	setDirection(dir);
}

Vector3D* DirectionalLight::getSceneDirection()
{
	if (m_sceneTransformDirty)
		updateSceneTransform();

	return &m_sceneDirection;
}

void DirectionalLight::setDirection(Vector3D& value)
{
	m_direction.copyFrom(value);

	Vector3D tmpLookAt(m_position.m_x + m_direction.m_x, m_position.m_y + m_direction.m_y, m_position.m_z + m_direction.m_z);
	lookAt(tmpLookAt);
}

void DirectionalLight::updateBounds()
{

}

void DirectionalLight::updateSceneTransform()
{
	LightBase::updateSceneTransform();
	m_sceneTransform.copyColumnTo(2, m_sceneDirection);
	m_sceneDirection.normalize();
}

EntityNode* DirectionalLight::createEntityPartitionNode()
{
	return new DirectionalLightNode(this);
}

ShadowMapperBase* DirectionalLight::createShadowMapper()
{
	return new DirectionalShadowMapper();
}

void DirectionalLight::getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result)
{
	Matrix3D matrix(renderable->getRenderSceneTransform(camera));
	matrix.append(getInverseSceneTransform());

	float projAABBPoints[24];
	BoundingVolumeBase* bounds = renderable->getSourceEntity()->getBounds();
	matrix.transformVectors(bounds->getAABBPoints(), projAABBPoints, 8);

	float xMin, yMin, zMin, xMax, yMax, zMax;
	xMin = yMin = zMin = MathConsts::Infinity;
	xMax = yMax = zMax = -MathConsts::Infinity;
	for (int i = 0; i < 24;)
	{
		float d = projAABBPoints[i++];
		if (d < xMin)
			xMin = d;
		if (d > xMax)
			xMax = d;

		d = projAABBPoints[i++];
		if (d < yMin)
			yMin = d;
		if (d > yMax)
			yMax = d;

		d = projAABBPoints[i++];
		if (d < zMin)
			zMin = d;
		if (d > zMax)
			zMax = d;
	}

	float invXRange = 1 / (xMax - xMin);
	float invYRange = 1 / (yMax - yMin);
	float invZRange = 1 / (zMax - zMin);
	float* raw = result.m_rawData;
	raw[0] = 2 * invXRange;
	raw[5] = 2 * invYRange;
	raw[10] = invZRange;
	raw[12] = -(xMax + xMin) * invXRange;
	raw[13] = -(yMax + yMin) * invYRange;
	raw[14] = -zMin * invZRange;
	raw[1] = raw[2] = raw[3] = raw[4] = raw[6] = raw[7] = raw[8] = raw[9] = raw[11] = 0;
	raw[15] = 1;
	result.prepend(matrix);
}