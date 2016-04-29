#include "View3D.h"
#include "Scene3D.h"
#include "Scene3DEvent.h"
#include "Camera3D.h"
#include "LensBase.h"
#include "DefaultRenderer.h"
#include "EntityCollector.h"
#include "IContext.h"
#include "Texture.h"
#include "RTTBufferManager.h"
#include "DepthRenderer.h"
#include "FilterRenderer.h"

USING_AWAY_NAMESPACE

View3D::View3D(Scene3D* scene, Camera3D* camera, RendererBase* renderer)
{
	m_visible = true;
	m_backBufferInvalid = true;
	m_requireDepthRender = false;
	m_filterRenderer = nullptr;
	m_depthRender = nullptr;
	m_x = m_y = m_width = m_height = 0;
	m_backgroundColor = 0;
	m_backgroundAlpha = 1;
	m_background = nullptr;
	m_antiAlias = 0;
	m_globalPosDirty = false;
	m_layeredView = false;
	m_scissorRectDirty = true;
	m_viewportDirty = true;
	m_depthTextureInvalid = true;

	m_scene = scene ? scene : new Scene3D();
	m_scene->addEventListener(Scene3DEvent::PARTITION_CHANGED, CALLBACK_OBJECT(onScenePartitionChanged));

	m_camera = camera ? camera : new Camera3D();
	// m_camera->addEventListener(CameraEvent::LENS_CHANGED, this, &View3D::onLensChanged);
	m_camera->setPartition(m_scene->getPartition());

	m_renderer = renderer ? renderer : new DefaultRenderer();
	m_depthRenderer = new DepthRenderer();

	m_entityCollector = m_renderer->createEntityCollector();
	m_entityCollector->setCamera(m_camera);
}

void View3D::setContext(IContext* value)
{
	m_context = value;

	m_rttBufferManager = RTTBufferManager::getInstance(m_context);

	m_renderer->setContext(m_context);

	m_globalPosDirty = true;
}

void View3D::setBackground(Texture2DBase* value)
{
	m_background = value;
	m_renderer->setBackground(value);
}

void View3D::setRenderer(RendererBase* value)
{
	if (value == m_renderer)
		return;

	m_renderer = value;
	m_renderer->setContext(m_context);
	m_renderer->setAntiAlias(m_antiAlias);
	m_renderer->setBackgroundR(((m_backgroundColor >> 16) & 0xff) / 255.f);
	m_renderer->setBackgroundG(((m_backgroundColor >> 8) & 0xff) / 255.f);
	m_renderer->setBackgroundB((m_backgroundColor & 0xff) / 255.f);
	m_renderer->setBackgroundA(m_backgroundAlpha);
	
	m_entityCollector = m_renderer->createEntityCollector();
	m_entityCollector->setCamera(m_camera);
}

void View3D::setBackgroundColor(unsigned int value)
{
	m_backgroundColor = value;
	m_renderer->setBackgroundR(((value >> 16) & 0xff) / 255.f);
	m_renderer->setBackgroundG(((value >> 8) & 0xff) / 255.f);
	m_renderer->setBackgroundB((value & 0xff) / 255.f);
}

void View3D::setBackgroundAlpha(float value)
{
	if (value > 1)
		value = 1;
	else if (value < 0)
		value = 0;

	m_backgroundAlpha = value;
	m_renderer->setBackgroundA(value);
}

void View3D::setCamera(Camera3D* value)
{
	// m_camera->removeEventListener(CameraEvent::LENS_CHANGED, this, &View3D::onLensChanged);

	m_camera = value;
	m_entityCollector->setCamera(m_camera);

	if (m_scene)
		m_camera->setPartition(m_scene->getPartition());

	// m_camera->addEventListener(CameraEvent::LENS_CHANGED, this, &View3D::onLensChanged);

	m_scissorRectDirty = true;
	m_viewportDirty = true;
}

void View3D::setScene(Scene3D* value)
{
	m_scene->removeEventListener(Scene3DEvent::PARTITION_CHANGED, CALLBACK_OBJECT(onScenePartitionChanged));
	m_scene = value;
	m_scene->addEventListener(Scene3DEvent::PARTITION_CHANGED, CALLBACK_OBJECT(onScenePartitionChanged));

	if (m_camera)
		m_camera->setPartition(m_scene->getPartition());
}

void View3D::setWidth(int value)
{
	if (value == m_width)
		return;

	m_rttBufferManager->setViewWidth(value);
	m_width = value;
	m_aspectRatio = (float)m_width / m_height;
	m_camera->getLens()->setAspectRatio(m_aspectRatio);
	m_depthTextureInvalid = true;

	m_scissorRect.m_width = value;

	m_backBufferInvalid = true;
	m_scissorRectDirty = true;
}

void View3D::setHeight(int value)
{
	if (value == m_height)
		return;

	m_rttBufferManager->setViewHeight(value);
	m_height = value;
	m_aspectRatio = (float)m_width / m_height;
	m_camera->getLens()->setAspectRatio(m_aspectRatio);
	m_depthTextureInvalid = true;

	m_scissorRect.m_height = value;

	m_backBufferInvalid = true;
	m_scissorRectDirty = true;
}

void View3D::setX(int value)
{
	if (value == m_x)
		return;

	m_x = value;
	m_globalPosDirty = true;
}

void View3D::setY(int value)
{
	if (value == m_y)
		return;

	m_y = value;
	m_globalPosDirty = true;
}

void View3D::setVisible(bool value)
{

}

void View3D::setAntiAlias(unsigned int value)
{
	if (value == m_antiAlias)
		return;

	m_antiAlias = value;
	m_renderer->setAntiAlias(value);
}

FilterVector& View3D::getFilters()
{
	return m_filterRenderer->getFilters();
}

void View3D::setFilters(FilterVector& value)
{
	if (value.size() > 0)
	{
		m_filterRenderer = new FilterRenderer(m_context);
		m_filterRenderer->setFilters(value);
		m_requireDepthRender = m_filterRenderer->requireDepthRender();
	}
	else
		m_filterRenderer = nullptr;
}

Texture* View3D::getSceneTexture()
{
	return m_filterRenderer->getMainInputTexture(m_context);
}

void View3D::updateBackBuffer()
{
	m_context->configureViewport(m_x, m_y, m_width, m_height);
	m_backBufferInvalid = false;
	m_viewportDirty = true;
}

void View3D::render()
{
	if (m_backBufferInvalid)
		updateBackBuffer();

	if (m_layeredView)
		m_context->clear(0, 0, 0, 1, 1, 0, ClearMask::DEPTH);

	if (m_globalPosDirty)
		updateGlobalPos();

	updateViewSizeData();

	// collect stuff to render
	m_entityCollector->clear();
	m_scene->traversePartitions(m_entityCollector);

	if (m_requireDepthRender)
		renderSceneDepthToTexture();

	// render
	if (m_filterRenderer && m_context)
	{
		m_renderer->render(m_entityCollector, m_filterRenderer->getMainInputTexture(m_context), m_rttBufferManager->getRenderToTextureRect());
		m_filterRenderer->render(m_context, m_camera, m_depthRender);
	}
	else
	{
		m_renderer->render(m_entityCollector, nullptr, &m_scissorRect);
	}
}

void View3D::project(Vector3D* point3d, Vector3D* result)
{
	m_camera->project(point3d, result);
	result->m_x = (result->m_x + 1.0f) * m_width * .5f;
	result->m_y = (result->m_y + 1.0f) * m_height * .5f;
}

void View3D::unproject(float sX, float sY, float sZ, Vector3D* result)
{
	m_camera->unproject((sX * 2 - m_width) / m_width, (sY * 2 - m_height) / m_height, sZ, result);
}

void View3D::getRay(float sX, float sY, float sZ, Vector3D* result)
{
	m_camera->getRay((sX * 2 - m_width) / m_width, (sY * 2 - m_height) / m_height, sZ, result);
}

void View3D::updateGlobalPos()
{
	m_globalPosDirty = false;
}

void View3D::updateViewSizeData()
{
	m_camera->getLens()->setAspectRatio(m_aspectRatio);

	if (m_scissorRectDirty)
	{
		m_scissorRectDirty = false;
		m_camera->getLens()->updateScissorRect(m_scissorRect.m_x, m_scissorRect.m_y, m_scissorRect.m_width, m_scissorRect.m_height);
	}

	if (m_viewportDirty)
	{
		m_viewportDirty = false;
		m_camera->getLens()->updateViewport(m_x, m_y, m_width, m_height);
	}

	if (m_filterRenderer)
	{
		m_renderer->setTextureRatioX(m_rttBufferManager->getTextureRatioX());
		m_renderer->setTextureRatioY(m_rttBufferManager->getTextureRatioY());
	}
	else
	{
		m_renderer->setTextureRatioX(1);
		m_renderer->setTextureRatioY(1);
	}
}

void View3D::renderDepthPrepass(EntityCollector* entityCollector)
{

}

void View3D::renderSceneDepthToTexture()
{
	if (m_depthTextureInvalid || !m_depthRender)
		initDepthTexture();

	m_depthRenderer->setTextureRatioX(m_rttBufferManager->getTextureRatioX());
	m_depthRenderer->setTextureRatioY(m_rttBufferManager->getTextureRatioY());
	m_depthRenderer->render(m_entityCollector, m_depthRender);
}

void View3D::initDepthTexture()
{
	m_depthTextureInvalid = false;
	m_depthRender = m_context->createTexture(m_rttBufferManager->getTextureWidth(), m_rttBufferManager->getTextureHeight(), TextureFormat::BGRA, true);
}

void View3D::onLensChanged(Event* event)
{
	m_scissorRectDirty = true;
	m_viewportDirty = true;
}

void View3D::onScenePartitionChanged(Event* event)
{
	if (m_camera)
		m_camera->setPartition(m_scene->getPartition());
}