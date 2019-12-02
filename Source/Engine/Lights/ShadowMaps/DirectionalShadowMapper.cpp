#include "DirectionalShadowMapper.h"
#include "Camera3D.h"
#include "ShadowCasterCollector.h"
#include "Scene3D.h"
#include "DepthRenderer.h"
#include "DirectionalLight.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

DirectionalShadowMapper::DirectionalShadowMapper()
{
	m_overallDepthCamera = new Camera3D(&m_overallDepthLens);
	m_lightOffset = 10000;
	m_snap = 64;
	m_minZ = 1;
}

DirectionalShadowMapper::~DirectionalShadowMapper()
{

}

Matrix3D& DirectionalShadowMapper::getDepthProjection()
{
	return m_overallDepthCamera->getViewProjection();
}

void DirectionalShadowMapper::updateDepthProjection(Camera3D* viewCamera)
{
	updateProjectionFromFrustumCorners(viewCamera, viewCamera->getLens()->getFrustumCorners(), m_matrix);
	m_overallDepthLens.setMatrix(m_matrix);
	updateCullPlanes(viewCamera);
}

void DirectionalShadowMapper::drawDepthMap(TextureBase* target, Scene3D* scene, DepthRenderer* renderer)
{
	m_casterCollector->setCamera(m_overallDepthCamera);
	m_casterCollector->setCullPlanes(&m_cullPlanes);
	m_casterCollector->clear();
	scene->traversePartitions(m_casterCollector);
	renderer->render(m_casterCollector, target);
}

void DirectionalShadowMapper::updateCullPlanes(Camera3D* viewCamera)
{
	PlaneVector& lightFrustumPlanes = *m_overallDepthCamera->getFrustumPlanes();
	PlaneVector& viewFrustumPlanes = *viewCamera->getFrustumPlanes();
	m_cullPlanes.resize(4);
	m_cullPlanes[0] = lightFrustumPlanes[0];
	m_cullPlanes[1] = lightFrustumPlanes[1];
	m_cullPlanes[2] = lightFrustumPlanes[2];
	m_cullPlanes[3] = lightFrustumPlanes[3];

	Vector3D* dir = ((DirectionalLight*)m_light)->getSceneDirection();
	float dirX = dir->m_x;
	float dirY = dir->m_y;
	float dirZ = dir->m_z;
	for (int i = 0; i < 6; i++)
	{
		Plane3D& plane = viewFrustumPlanes[i];
		if (plane.m_a * dirX + plane.m_b * dirY + plane.m_c * dirZ < 0)
			m_cullPlanes.push_back(plane);
	}
}

void DirectionalShadowMapper::updateProjectionFromFrustumCorners(Camera3D* viewCamera, float* corners, Matrix3D& matrix)
{
	Vector3D* dir = static_cast<DirectionalLight*>(m_light)->getSceneDirection();
	m_overallDepthCamera->setTransform(m_light->getSceneTransform());
	float x = int((viewCamera->getX() - dir->m_x * m_lightOffset) / m_snap) * m_snap;
	float y = int((viewCamera->getY() - dir->m_y * m_lightOffset) / m_snap) * m_snap;
	float z = int((viewCamera->getZ() - dir->m_z * m_lightOffset) / m_snap) * m_snap;
	m_overallDepthCamera->setX(x);
	m_overallDepthCamera->setY(y);
	m_overallDepthCamera->setZ(z);

	m_matrix.copyFrom(m_overallDepthCamera->getInverseSceneTransform());
	m_matrix.prepend(viewCamera->getSceneTransform());
	m_matrix.transformVectors(corners, m_localFrustum, 8);

	float minX, minY, maxX, maxY;
	minX = maxX = m_localFrustum[0];
	minY = maxY = m_localFrustum[1];
	m_maxZ = m_localFrustum[2];

	for (int i = 3; i < 24; i += 3)
	{
		x = m_localFrustum[i];
		y = m_localFrustum[i + 1];
		z = m_localFrustum[i + 2];
		if (x < minX)
			minX = x;
		if (x > maxX)
			maxX = x;
		if (y < minY)
			minY = y;
		if (y > maxY)
			maxY = y;
		if (z > m_maxZ)
			m_maxZ = z;
	}

	float w = maxX - minX;
	float h = maxY - minY;
	float d = 1 / (m_maxZ - m_minZ);
	
	if (minX < 0)
		minX -= m_snap; // because int() rounds up for < 0
	if (minY < 0)
		minY -= m_snap;

	minX = int(minX / m_snap) * m_snap;
	minY = int(minY / m_snap) * m_snap;

	float snap2 = m_snap * 2;
	w = int(w / snap2 + 2) * snap2;
	h = int(h / snap2 + 2) * snap2;

	maxX = minX + w;
	maxY = minY + h;

	w = 1 / w;
	h = 1 / h;

	float* raw = matrix.m_rawData;
	raw[0] = 2 * w;
	raw[5] = 2 * h;
	raw[10] = d;
	raw[12] = -(maxX + minX) * w;
	raw[13] = -(maxY + minY) * h;
	raw[14] = -m_minZ * d;
	raw[15] = 1;
	raw[1] = raw[2] = raw[3] = raw[4] = raw[6] = raw[7] = raw[8] = raw[9] = raw[11] = 0;
}