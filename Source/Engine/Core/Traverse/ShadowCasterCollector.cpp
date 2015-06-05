#include "ShadowCasterCollector.h"
#include "IRenderable.h"
#include "MaterialBase.h"
#include "Entity.h"

USING_AWAY_NAMESPACE

void ShadowCasterCollector::applyRenderable(IRenderable* renderable)
{
	MaterialBase* material = renderable->getMaterial();
	if (renderable->getCastsShadows() && material)
	{
		RenderableListItem* item = m_renderableListItemPool.getItem();
		item->m_renderable = renderable;
		item->m_next = m_opaqueRenderableHead;
		item->m_cascaded = false;
		Vector3D* entityPoint = renderable->getSourceEntity()->getScenePosition();
		float dx = m_entryPoint->m_x - entityPoint->m_x;
		float dy = m_entryPoint->m_y - entityPoint->m_y;
		float dz = m_entryPoint->m_z - entityPoint->m_z;
		item->m_zIndex = dx * m_cameraForward.m_x + dy * m_cameraForward.m_y + dz * m_cameraForward.m_z;
		item->m_renderOrderId = material->getDepthPassId();
		m_opaqueRenderableHead = item;
	}
}

void ShadowCasterCollector::applyUnknownLight(LightBase* light)
{

}

void ShadowCasterCollector::applyDirectionalLight(DirectionalLight* light)
{

}

void ShadowCasterCollector::applyPointLight(PointLight* light)
{

}

void ShadowCasterCollector::applyLightProbe(LightProbe* light)
{

}

void ShadowCasterCollector::applySkyBox(IRenderable* renderable)
{

}