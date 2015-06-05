#include "BackgroundImageRenderer.h"
#include "Texture2DBase.h"
#include "ByteArray.h"
#include "IContext.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Program.h"
#include "Shader.h"
#include "Regs.h"
#include "Tex.h"

USING_AWAY_NAMESPACE

BackgroundImageRenderer::BackgroundImageRenderer(IContext* context)
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	setContext(context);
}

void BackgroundImageRenderer::setContext(IContext* value)
{
	if (value != m_context)
	{
		m_context = value;
		removeBuffers();
	}
}

void BackgroundImageRenderer::render()
{
	if (!m_context)
		return;

	if (!m_vertexBuffer)
		initBuffers();

	m_context->setBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);
	m_context->setProgram(m_program);
	m_context->setTextureAt(0, m_texture->getTextureForContext(m_context));
	m_context->setVertexBufferAt(0, m_vertexBuffer, 0, VertexBufferFormat::FLOAT_2);
	m_context->setVertexBufferAt(1, m_vertexBuffer, 8, VertexBufferFormat::FLOAT_2);
	m_context->drawTriangles(m_indexBuffer, 0, 2);
	m_context->setVertexBufferAt(0, nullptr);
	m_context->setVertexBufferAt(1, nullptr);
	m_context->setTextureAt(0, nullptr);
}

void BackgroundImageRenderer::initBuffers()
{
	// create VertexBuffer
	float w = 2, h = 2, x = -1, y = 1;
	float vertices[16] = { x, y - h, 0, 1, x + w, y - h, 1, 1, x + w, y, 1, 0, x, y, 0, 0 };
	m_vertexBuffer = m_context->createVertexBuffer(4, 16);
	m_vertexBuffer->uploadFromVector(vertices, 0, 4);

	// create IndexBuffer
	unsigned short indices[6] = { 2, 1, 0, 3, 2, 0 };
	m_indexBuffer = m_context->createIndexBuffer(6);
	m_indexBuffer->uploadFromVector(indices, 0, 6);

	// create vertex shader
	Shader vertexShader(ProgramType::VERTEX);
	vertexShader.mov(Regs::op, Regs::a0);
	vertexShader.mov(Regs::v0, Regs::a1);
	ByteArray vertexShaderBytes;
	vertexShader.writeBytes(vertexShaderBytes);

	// create fragment shader
	Shader fragmentShader(ProgramType::FRAGMENT);
	fragmentShader.tex(Regs::t0, Regs::v0, Regs::s0);
	fragmentShader.mov(Regs::oc, Regs::t0);
	ByteArray fragmentShaderBytes;
	fragmentShader.writeBytes(fragmentShaderBytes);

	// create program
	m_program = m_context->createProgram();
	m_program->upload(vertexShaderBytes, fragmentShaderBytes);
}

void BackgroundImageRenderer::removeBuffers()
{

}