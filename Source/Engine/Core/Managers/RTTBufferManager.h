#pragma once

#include "Common.h"
#include "EventDispatcher.h"
#include "Rectangle.h"

AWAY_NAMESPACE_BEGIN

class RTTBufferManager : public EventDispatcher
{
public:
	static RTTBufferManager* getInstance(IContext* context);
	float getTextureRatioX();
	float getTextureRatioY();
	int getViewWidth() { return m_viewWidth; }
	void setViewWidth(int value);
	int getViewHeight() { return m_viewHeight; }
	void setViewHeight(int value);
	VertexBuffer* getRenderToTextureVertexBuffer();
	VertexBuffer* getRenderToScreenVertexBuffer();
	IndexBuffer* getIndexBuffer() { return m_indexBuffer; }
	Rectangle* getRenderToTextureRect();
	int getTextureWidth() { return m_textureWidth; }
	int getTextureHeight() { return m_textureHeight; }

private:
	RTTBufferManager(IContext* context);
	void updateRTTBuffers();

private:
	static RTTBufferManager* m_instance;
	VertexBuffer* m_renderToTextureVertexBuffer;
	VertexBuffer* m_renderToScreenVertexBuffer;
	IndexBuffer* m_indexBuffer;
	IContext* m_context;
	int m_viewWidth;
	int m_viewHeight;
	int m_textureWidth;
	int m_textureHeight;
	Rectangle m_renderToTextureRect;
	bool m_buffersInvalid;
	float m_textureRatioX;
	float m_textureRatioY;
};

AWAY_NAMESPACE_END