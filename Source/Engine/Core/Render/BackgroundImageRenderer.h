#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class BackgroundImageRenderer
{
public:
	BackgroundImageRenderer(IContext* context);

	IContext* getContext() { return m_context; }
	void setContext(IContext* value);

	Texture2DBase* getTexture() { return m_texture; }
	void setTexture(Texture2DBase* value) { m_texture = value; }

	void render();

private:
	void initBuffers();
	void removeBuffers();

private:
	IContext* m_context;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	Program* m_program;
	Texture2DBase* m_texture;
};

AWAY_NAMESPACE_END