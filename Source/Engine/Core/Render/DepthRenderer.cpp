#include "DepthRenderer.h"
#include "IContext.h"
#include "EntityCollector.h"
#include "MaterialBase.h"
#include "IRenderable.h"

USING_AWAY_NAMESPACE

DepthRenderer::DepthRenderer(bool renderBlended, bool distanceBased)
{
	m_activeMaterial = nullptr;
	m_renderBlended = renderBlended;
	m_distanceBased = distanceBased;
	m_disableColor = false;
	m_backgroundR = 1;
	m_backgroundG = 1;
	m_backgroundB = 1;
}

void DepthRenderer::draw(EntityCollector* entityCollector, TextureBase* target)
{
	m_context->setBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);
	m_context->setDepthTest(true, CompareMode::LESS);
	drawRenderables(entityCollector->getOpaqueRenderableHead(), entityCollector);

	if (m_disableColor)
		m_context->setColorMask(false, false, false, false);

	if (m_renderBlended)
		drawRenderables(entityCollector->getBlendedRenderableHead(), entityCollector);

	if (m_activeMaterial)
	{
		m_activeMaterial->deactivateForDepth(m_context);
		m_activeMaterial = nullptr;
	}

	if (m_disableColor)
		m_context->setColorMask(true, true, true, true);
}

void DepthRenderer::drawRenderables(RenderableListItem* item, EntityCollector* entityCollector)
{
	RenderableListItem* item2;
	Camera3D* camera = entityCollector->getCamera();
	while (item)
	{
		m_activeMaterial = item->m_renderable->getMaterial();

		// otherwise this would result in depth rendered anyway because fragment shader kil is ignored
		if (m_disableColor && m_activeMaterial->hasDepthAlphaThreshold())
		{
			item2 = item;
			do 
			{
				item2 = item2->m_next;
			} while (item2 && item2->m_renderable->getMaterial() == m_activeMaterial);
		}
		else
		{
			m_activeMaterial->activateForDepth(m_context, camera, m_distanceBased);
			item2 = item;
			do 
			{
				m_activeMaterial->renderDepth(item2->m_renderable, m_context, camera, m_rttViewProjectionMatrix);
				item2 = item2->m_next;
			} while (item2 && item2->m_renderable->getMaterial() == m_activeMaterial);
			m_activeMaterial->deactivateForDepth(m_context);
		}

		item = item2;
	}
}