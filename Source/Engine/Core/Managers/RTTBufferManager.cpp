#include "RTTBufferManager.h"
#include "TextureUtils.h"
#include "Event.h"
#include "IContext.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

USING_AWAY_NAMESPACE

RTTBufferManager* RTTBufferManager::m_instance = nullptr;

RTTBufferManager::RTTBufferManager(IContext* context)
{
	m_renderToTextureVertexBuffer = nullptr;
	m_renderToScreenVertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_context = context;
	m_viewWidth = -1;
	m_viewHeight = -1;
	m_textureWidth = -1;
	m_textureHeight = -1;
	m_buffersInvalid = true;
}

RTTBufferManager* RTTBufferManager::getInstance(IContext* context)
{
	if (!m_instance)
		m_instance = new RTTBufferManager(context);

	return m_instance;
}

float RTTBufferManager::getTextureRatioX()
{
	if (m_buffersInvalid)
		updateRTTBuffers();

	return m_textureRatioX;
}

float RTTBufferManager::getTextureRatioY()
{
	if (m_buffersInvalid)
		updateRTTBuffers();

	return m_textureRatioY;
}

void RTTBufferManager::setViewWidth(int value)
{
	if (m_viewWidth == value)
		return;

	m_viewWidth = value;
	m_buffersInvalid = true;
	m_textureWidth = TextureUtils::getBestPowerOf2(value);

	if (m_textureWidth > m_viewWidth)
	{
		m_renderToTextureRect.m_x = (m_textureWidth - m_viewWidth) / 2;
		m_renderToTextureRect.m_width = m_viewWidth;
	}
	else
	{
		m_renderToTextureRect.m_x = 0;
		m_renderToTextureRect.m_width = m_textureWidth;
	}

	Event event(Event::RESIZE);
	dispatchEvent(&event);
}

void RTTBufferManager::setViewHeight(int value)
{
	if (m_viewHeight == value)
		return;

	m_viewHeight = value;
	m_buffersInvalid = true;
	m_textureHeight = TextureUtils::getBestPowerOf2(value);

	if (m_textureHeight > m_viewHeight)
	{
		m_renderToTextureRect.m_y = (m_textureHeight - m_viewHeight) / 2;
		m_renderToTextureRect.m_height = m_viewHeight;
	}
	else
	{
		m_renderToTextureRect.m_y = 0;
		m_renderToTextureRect.m_height = m_textureHeight;
	}

	Event event(Event::RESIZE);
	dispatchEvent(&event);
}

VertexBuffer* RTTBufferManager::getRenderToTextureVertexBuffer()
{
	if (m_buffersInvalid)
		updateRTTBuffers();

	return m_renderToTextureVertexBuffer;
}

VertexBuffer* RTTBufferManager::getRenderToScreenVertexBuffer()
{
	if (m_buffersInvalid)
		updateRTTBuffers();

	return m_renderToScreenVertexBuffer;
}

Rectangle* RTTBufferManager::getRenderToTextureRect()
{
	if (m_buffersInvalid)
		updateRTTBuffers();

	return &m_renderToTextureRect;
}

void RTTBufferManager::updateRTTBuffers()
{
	if (!m_renderToTextureVertexBuffer)
		m_renderToTextureVertexBuffer = m_context->createVertexBuffer(4, 16);

	if (!m_renderToScreenVertexBuffer)
		m_renderToScreenVertexBuffer = m_context->createVertexBuffer(4, 16);

	if (!m_indexBuffer)
	{
		unsigned short indices[] = { 2, 1, 0, 3, 2, 0 };
		m_indexBuffer = m_context->createIndexBuffer(6);
		m_indexBuffer->uploadFromVector(indices, 0, 6);
	}
	
	m_textureRatioX = std::min((float)m_viewWidth / m_textureWidth, 1.f);
	m_textureRatioY = std::min((float)m_viewHeight / m_textureHeight, 1.f);
	
	float u1 = (1 - m_textureRatioX) * .5f;
	float u2 = (m_textureRatioX + 1) * .5f;
	float v1 = (m_textureRatioY + 1) * .5f;
	float v2 = (1 - m_textureRatioY) * .5f;

	float rttVertices[] = { -m_textureRatioX, -m_textureRatioY, u1, v1, // bottom left
							m_textureRatioX, -m_textureRatioY, u2, v1, // bottom right
							m_textureRatioX, m_textureRatioY, u2, v2, // top right
							-m_textureRatioX, m_textureRatioY, u1, v2 }; // top left
	float rtsVertices[] = { -1, -1, u1, v1, // bottom left
							1, -1, u2, v1, // bottom right
							1, 1, u2, v2, // top right
							-1, 1, u1, v2 }; // top left
	m_renderToTextureVertexBuffer->uploadFromVector(rttVertices, 0, 4);
	m_renderToScreenVertexBuffer->uploadFromVector(rtsVertices, 0, 4);
	m_buffersInvalid = false;
}