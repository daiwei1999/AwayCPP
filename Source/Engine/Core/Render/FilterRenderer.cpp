#include "FilterRenderer.h"
#include "RTTBufferManager.h"
#include "Event.h"
#include "FilterBase.h"
#include "FilterTaskBase.h"
#include "IContext.h"
#include "Texture.h"

USING_AWAY_NAMESPACE

FilterRenderer::FilterRenderer(IContext* context)
{
	m_filterTasksInvalid = false;
	m_filterSizesInvalid = true;
	m_context = context;
	m_rttManager = RTTBufferManager::getInstance(context);
	m_rttManager->addEventListener(Event::RESIZE, CALLBACK_OBJECT(onRTTResize));
}

Texture* FilterRenderer::getMainInputTexture(IContext* context)
{
	if (m_filterTasksInvalid)
		updateFilterTasks(context);

	return m_mainInputTexture;
}

void FilterRenderer::setFilters(FilterVector& value)
{
	m_filters = value;
	m_filterTasksInvalid = true;
	m_requireDepthRender = false;
	m_filterSizesInvalid = true;

	for (auto filter : m_filters)
	{
		if (filter->requireDepthRender())
		{
			m_requireDepthRender = true;
			break;
		}
	}
}

void FilterRenderer::render(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	if (m_filterSizesInvalid)
		updateFilterSizes();

	if (m_filterTasksInvalid)
		updateFilterTasks(context);

	for (auto filter : m_filters)
		filter->update(context, camera);

	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer = m_rttManager->getIndexBuffer();
	for (auto task : m_tasks)
	{
		Texture* target = task->getTarget();
		if (target)
		{
			context->setRenderToTexture(target);
			context->setProgram(task->getProgram(context));
			vertexBuffer = m_rttManager->getRenderToTextureVertexBuffer();
			context->setVertexBufferAt(0, vertexBuffer, 0, VertexBufferFormat::FLOAT_2);
			context->setVertexBufferAt(1, vertexBuffer, 8, VertexBufferFormat::FLOAT_2);
		}
		else
		{
			context->setRenderToBackBuffer();
			context->setScissorRectangle(nullptr);
			context->setProgram(task->getProgram(context));
			vertexBuffer = m_rttManager->getRenderToScreenVertexBuffer();
			context->setVertexBufferAt(0, vertexBuffer, 0, VertexBufferFormat::FLOAT_2);
			context->setVertexBufferAt(1, vertexBuffer, 8, VertexBufferFormat::FLOAT_2);
		}

		context->setSamplerStateAt(0, WrapMode::CLAMP, TextureFilter::LINEAR, MipFilter::MIPNONE);
		context->setTextureAt(0, task->getMainInputTexture(context));
		context->clear(0, 0, 0, 0);
		task->activate(context, camera, depthTexture);
		context->setBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);
		context->drawTriangles(indexBuffer, 0, 2);
		task->deactivate(context);
	}

	context->setTextureAt(0, nullptr);
	context->setVertexBufferAt(0, nullptr);
	context->setVertexBufferAt(1, nullptr);
}

void FilterRenderer::updateFilterTasks(IContext* context)
{
	if (m_filterSizesInvalid)
		updateFilterSizes();

	m_tasks.clear();

	FilterBase* filter;
	size_t len = m_filters.size() - 1;
	for (size_t i = 0; i <= len; i++)
	{
		filter = m_filters[i];
		filter->setRenderTargets(i == len ? nullptr : m_filters[i + 1]->getMainInputTexture(context), context);
		const std::vector<FilterTaskBase*>& tasks = filter->getTasks();
		m_tasks.insert(m_tasks.end(), tasks.begin(), tasks.end());
	}

	m_mainInputTexture = m_filters[0]->getMainInputTexture(context);
}

void FilterRenderer::updateFilterSizes()
{
	int textureWidth = m_rttManager->getTextureWidth();
	int textureHeight = m_rttManager->getTextureHeight();
	for (auto filter : m_filters)
	{
		filter->setTextureWidth(textureWidth);
		filter->setTextureHeight(textureHeight);
	}

	m_filterSizesInvalid = false;
}

void FilterRenderer::onRTTResize(Event* event)
{
	m_filterSizesInvalid = true;
}