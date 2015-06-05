#include "EntityCollector.h"
#include "Camera3D.h"
#include "NodeBase.h"
#include "IRenderable.h"
#include "RenderableListItemPool.h"
#include "MaterialBase.h"
#include "Plane3D.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "LightProbe.h"

USING_AWAY_NAMESPACE

EntityCollector::EntityCollector()
{
	m_skyBox = nullptr;
	m_opaqueRenderableHead = m_blendedRenderableHead = nullptr;
	m_numLights = m_numDirectionalLights = m_numPointLights = m_numLightProbes = 0;
	m_numTriangles = m_numMouseEnableds = 0;
	m_camera = nullptr;
	m_entityHead = nullptr;
	m_cullPlanes = nullptr;
	m_customCullPlanes = nullptr;
	m_numCullPlanes = 0;
}

void EntityCollector::clear()
{
	if (m_camera)
	{
		m_entryPoint = m_camera->getScenePosition();
		m_camera->getTransform().getForward(m_cameraForward);
	}
	m_cullPlanes = m_customCullPlanes ? m_customCullPlanes : (m_camera ? m_camera->getFrustumPlanes() : nullptr);
	m_numCullPlanes = m_cullPlanes ? m_cullPlanes->size() : 0;
	m_numLights = m_numDirectionalLights = m_numPointLights = m_numLightProbes = 0;
	m_numTriangles = m_numMouseEnableds = 0;
	m_blendedRenderableHead = m_opaqueRenderableHead = nullptr;
	m_entityHead = nullptr;
	m_renderableListItemPool.freeAll();
	m_entityListItemPool.freeAll();
	m_skyBox = nullptr;

	m_lights.clear();
	m_directionalLights.clear();
	m_pointLights.clear();
	m_lightProbes.clear();
}

bool EntityCollector::enterNode(NodeBase* node)
{
	bool enter = (m_collectionMark != node->getCollectionMark() && node->isInFrustum(m_cullPlanes, m_numCullPlanes));
	node->setCollectionMark(m_collectionMark);
	return enter;
}

void EntityCollector::applyRenderable(IRenderable* renderable)
{
	if (renderable->getMouseEnabled())
		++m_numMouseEnableds;

	m_numTriangles += renderable->getTriangleCount();
	MaterialBase* material = renderable->getMaterial();
	if (material)
	{
		RenderableListItem* item = m_renderableListItemPool.getItem();
		item->m_renderable = renderable;
		item->m_materialId = material->getUniqueId();
		item->m_renderOrderId = material->getRenderOrderId();
		item->m_cascaded = false;

		// project onto camera's z-axis
		Entity* entity = renderable->getSourceEntity();
		Vector3D* entityPoint = entity->getScenePosition();
		float dx = m_entryPoint->m_x - entityPoint->m_x;
		float dy = m_entryPoint->m_y - entityPoint->m_y;
		float dz = m_entryPoint->m_z - entityPoint->m_z;
		item->m_zIndex = dx * m_cameraForward.m_x + dy * m_cameraForward.m_y + dz * m_cameraForward.m_z + entity->getZOffset();
		if (material->requiresBlending())
		{
			item->m_next = m_blendedRenderableHead;
			m_blendedRenderableHead = item;
		}
		else
		{
			item->m_next = m_opaqueRenderableHead;
			m_opaqueRenderableHead = item;
		}
	}
}

void EntityCollector::applyUnknownLight(LightBase* light)
{
	m_lights.push_back(light);
	m_numLights++;
}

void EntityCollector::applyDirectionalLight(DirectionalLight* light)
{
	m_lights.push_back(light);
	m_numLights++;
	m_directionalLights.push_back(light);
	m_numDirectionalLights++;
}

void EntityCollector::applyPointLight(PointLight* light)
{
	m_lights.push_back(light);
	m_numLights++;
	m_pointLights.push_back(light);
	m_numPointLights++;
}

void EntityCollector::applyLightProbe(LightProbe* light)
{
	m_lights.push_back(light);
	m_numLights++;
	m_lightProbes.push_back(light);
	m_numLightProbes++;
}

void EntityCollector::applyEntity(Entity* entity)
{
	EntityListItem* item = m_entityListItemPool.getItem();
	item->m_entity = entity;
	item->m_next = m_entityHead;
	m_entityHead = item;
}