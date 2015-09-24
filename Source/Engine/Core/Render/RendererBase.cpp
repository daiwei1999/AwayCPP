#include "RendererBase.h"
#include "RenderableMergeSort.h"
#include "EntityCollector.h"
#include "Camera3D.h"
#include "IContext.h"
#include "BackgroundImageRenderer.h"

USING_AWAY_NAMESPACE

RendererBase::RendererBase()
{
	m_renderableSorter = new RenderableMergeSort();
	m_backgroundR = m_backgroundG = m_backgroundB = 0;
	m_backgroundA = 1;
	m_shareContext = true;
	m_textureRatioX = m_textureRatioY = 1;
	m_backgroundImageRenderer = nullptr;
	m_clearOnRender = true;
}

void RendererBase::setContext(IContext* value)
{
	if (value == m_context)
		return;

	m_context = value;

	if (m_backgroundImageRenderer)
		m_backgroundImageRenderer->setContext(m_context);
}

void RendererBase::setBackground(Texture2DBase* value)
{
	if (m_backgroundImageRenderer && !value)
	{
		delete m_backgroundImageRenderer;
		m_backgroundImageRenderer = nullptr;
	}
	else if (!m_backgroundImageRenderer && value)
	{
		m_backgroundImageRenderer = new BackgroundImageRenderer(m_context);
	}

	m_background = value;
	if (m_backgroundImageRenderer)
		m_backgroundImageRenderer->setTexture(m_background);
}

EntityCollector* RendererBase::createEntityCollector()
{
	return new EntityCollector();
}

void RendererBase::render(EntityCollector* entityCollector, TextureBase* target, Rectangle<int>* scissorRect, int surfaceSelector)
{
	if (!m_context)
		return;

	m_rttViewProjectionMatrix.copyFrom(entityCollector->getCamera()->getViewProjection());
	m_rttViewProjectionMatrix.appendScale(m_textureRatioX, m_textureRatioY, 1);

	executeRender(entityCollector, target, scissorRect, surfaceSelector);

	// clear buffers
	for (int i = 0; i < 8; i++)
	{
		m_context->setVertexBufferAt(i, nullptr);
		m_context->setTextureAt(i, nullptr);
	}
}

void RendererBase::executeRender(EntityCollector* entityCollector, TextureBase* target, Rectangle<int>* scissorRect, int surfaceSelector)
{
	if (m_renderableSorter)
		m_renderableSorter->sort(entityCollector);

	if (target)
		m_context->setRenderToTexture(target, true, m_antiAlias, surfaceSelector);
	else
		m_context->setRenderToBackBuffer();

	if ((target || !m_shareContext) && m_clearOnRender)
		m_context->clear(m_backgroundR, m_backgroundG, m_backgroundB, m_backgroundA, 1, 0);

	m_context->setDepthTest(false, CompareMode::ALWAYS);
	m_context->setScissorRectangle(scissorRect);

	if (m_backgroundImageRenderer)
		m_backgroundImageRenderer->render();

	draw(entityCollector, target);

	// line required for correct rendering when using away3d with starling. DO NOT REMOVE UNLESS STARLING INTEGRATION IS RETESTED!
	//m_context->setDepthTest(false, CompareMode::LESS_EQUAL);

	m_context->setScissorRectangle(nullptr);
}