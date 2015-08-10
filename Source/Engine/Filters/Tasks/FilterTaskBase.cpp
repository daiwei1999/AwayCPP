#include "FilterTaskBase.h"
#include "IContext.h"
#include "Program.h"
#include "ByteArray.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

FilterTaskBase::FilterTaskBase(bool requireDepthRender)
{
	m_mainInputTexture = nullptr;
	m_mainInputTextureContext = nullptr;
	m_scaledTextureWidth = -1;
	m_scaledTextureHeight = -1;
	m_textureWidth = -1;
	m_textureHeight = -1;
	m_textureScale = 0;
	m_requireDepthRender = requireDepthRender;
	m_textureDimensionsInvalid = true;
	m_programInvalid = true;
	m_program = nullptr;
	m_programContext = nullptr;
	m_target = nullptr;
}

void FilterTaskBase::setTextureScale(int value)
{
	if (m_textureScale == value)
		return;

	m_textureScale = value;
	m_scaledTextureWidth = m_textureWidth >> m_textureScale;
	m_scaledTextureHeight = m_textureHeight >> m_textureScale;
	m_textureDimensionsInvalid = true;
}

void FilterTaskBase::setTextureWidth(int value)
{
	if (m_textureWidth == value)
		return;

	m_textureWidth = value;
	m_scaledTextureWidth = m_textureWidth >> m_textureScale;
	if (m_scaledTextureWidth < 1) m_scaledTextureWidth = 1;
	m_textureDimensionsInvalid = true;
}

void FilterTaskBase::setTextureHeight(int value)
{
	if (m_textureHeight == value)
		return;

	m_textureHeight = value;
	m_scaledTextureHeight = m_textureHeight >> m_textureScale;
	if (m_scaledTextureHeight < 1) m_scaledTextureHeight = 1;
	m_textureDimensionsInvalid = true;
}

Texture* FilterTaskBase::getMainInputTexture(IContext* context)
{
	if (context != m_mainInputTextureContext)
		m_textureDimensionsInvalid = true;

	if (m_textureDimensionsInvalid)
		updateTextures(context);

	return m_mainInputTexture;
}

Program* FilterTaskBase::getProgram(IContext* context)
{
	if (m_programContext != context)
		m_programInvalid = true;

	if (m_programInvalid)
	{
		// create vertex shader
		Shader vertexShader(ProgramType::VERTEX);
		getVertexCode(vertexShader);
		ByteArray vertexShaderBytes;
		vertexShader.writeBytes(vertexShaderBytes);

		// create fragment shader
		Shader fragmentShader(ProgramType::FRAGMENT);
		getFragmentCode(fragmentShader);
		ByteArray fragmentShaderBytes;
		fragmentShader.writeBytes(fragmentShaderBytes);

		// create program
		m_program = context->createProgram();
		m_program->upload(vertexShaderBytes, fragmentShaderBytes);
		m_programContext = context;
		m_programInvalid = false;
	}

	return m_program;
}

void FilterTaskBase::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{

}

void FilterTaskBase::deactivate(IContext* context)
{

}

void FilterTaskBase::getVertexCode(Shader& vertexShader)
{
	vertexShader.mov(Regs::op, Regs::a0);
	vertexShader.mov(Regs::v0, Regs::a1);
}

void FilterTaskBase::updateTextures(IContext* context)
{
	m_mainInputTextureContext = context;
	m_mainInputTexture = context->createTexture(m_scaledTextureWidth, m_scaledTextureHeight, TextureFormat::BGRA, true);
	m_textureDimensionsInvalid = false;
}