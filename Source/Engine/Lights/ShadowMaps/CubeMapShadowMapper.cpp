#include "CubeMapShadowMapper.h"
#include "RenderCubeTexture.h"
#include "PerspectiveLens.h"
#include "PointLight.h"
#include "ShadowCasterCollector.h"
#include "Scene3D.h"
#include "DepthRenderer.h"
#include "Plane3D.h"

USING_AWAY_NAMESPACE

CubeMapShadowMapper::CubeMapShadowMapper()
{
	m_depthMapSize = 512;

	// posX, negX, posY, negY, posZ, negZ
	setCameraOrientation(m_depthCameras[0], 0, 90, 0);
	setCameraOrientation(m_depthCameras[1], 0, -90, 0);
	setCameraOrientation(m_depthCameras[2], -90, 0, 0);
	setCameraOrientation(m_depthCameras[3], 90, 0, 0);
	setCameraOrientation(m_depthCameras[4], 0, 0, 0);
	setCameraOrientation(m_depthCameras[5], 0, 180, 0);
}

TextureProxyBase* CubeMapShadowMapper::createDepthTexture()
{
	return new RenderCubeTexture(m_depthMapSize);
}

void CubeMapShadowMapper::updateDepthProjection(Camera3D* viewCamera)
{
	float maxDistance = static_cast<PointLight*>(m_light)->getFallOff();
	Vector3D* pos = m_light->getScenePosition();

	// todo: faces outside frustum which are pointing away from camera need not be rendered!
	for (int i = 0; i < 6; i++)
	{
		m_depthCameras[i].getLens()->setFar(maxDistance);
		m_depthCameras[i].setPosition(pos);
		m_needsRender[i] = true;
	}
}

void CubeMapShadowMapper::drawDepthMap(TextureBase* target, Scene3D* scene, DepthRenderer* renderer)
{
	for (int i = 0; i < 6; i++)
	{
		if (m_needsRender[i])
		{
			m_casterCollector->setCamera(&m_depthCameras[i]);
			m_casterCollector->clear();
			scene->traversePartitions(m_casterCollector);
			renderer->render(m_casterCollector, target, nullptr, i);
		}
	}
}

void CubeMapShadowMapper::setCameraOrientation(Camera3D& camera, float rotationX, float rotationY, float rotationZ)
{
	camera.setRotationX(rotationX);
	camera.setRotationY(rotationY);
	camera.setRotationZ(rotationZ);

	PerspectiveLens* lens = (PerspectiveLens*)camera.getLens();
	lens->setNear(.01f);
	lens->setFieldOfView(90);
	lens->setAspectRatio(1);
}