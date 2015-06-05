#include "OpenGLES2Context.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Program.h"
#include "Texture.h"
#include "Matrix3D.h"
#include "Rectangle.h"

#if AWAY_PLATFORM == AWAY_PLATFORM_WINDOWS
# if defined(AWAY_PLUGIN_ANGLE)
#  include "GLES2/gl2.h"
# else
#  include "glew.h"
# endif
#elif AWAY_PLATFORM == AWAY_PLATFORM_OSX
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_LINUX
# include <GL/gl.h>
# include <GL/glu.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_IOS
# include <OpenGLES/ES2/gl.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
# include <GLES2/gl2.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_HTML5
# include <GLES2/gl2.h>
# include <EGL/egl.h>
#endif

USING_AWAY_NAMESPACE

OpenGLES2Context::TriangleFaceMap OpenGLES2Context::m_triangleFaces = { { TriangleFace::BACK, GL_BACK },
																		{ TriangleFace::FRONT, GL_FRONT },
																		{ TriangleFace::FRONT_AND_BACK, GL_FRONT_AND_BACK } };

OpenGLES2Context::BlendFactorMap OpenGLES2Context::m_blendFactors = { { BlendFactor::DESTINATION_ALPHA, GL_DST_ALPHA },
																	  { BlendFactor::DESTINATION_COLOR, GL_DST_COLOR },
																	  { BlendFactor::ONE, GL_ONE },
																	  { BlendFactor::ONE_MINUS_DESTINATION_ALPHA, GL_ONE_MINUS_DST_ALPHA },
																	  { BlendFactor::ONE_MINUS_DESTINATION_COLOR, GL_ONE_MINUS_DST_COLOR },
																	  { BlendFactor::ONE_MINUS_SOURCE_ALPHA, GL_ONE_MINUS_SRC_ALPHA },
																	  { BlendFactor::ONE_MINUS_SOURCE_COLOR, GL_ONE_MINUS_SRC_COLOR },
																	  { BlendFactor::SOURCE_ALPHA, GL_SRC_ALPHA },
																	  { BlendFactor::SOURCE_COLOR, GL_SRC_COLOR },
																	  { BlendFactor::ZERO, GL_ZERO } };

OpenGLES2Context::CompareFuncMap OpenGLES2Context::m_compareFuncs = { { CompareMode::ALWAYS, GL_ALWAYS },
																	  { CompareMode::EQUAL, GL_EQUAL },
																	  { CompareMode::GREATER, GL_GREATER },
																	  { CompareMode::GREATER_EQUAL, GL_GEQUAL },
																	  { CompareMode::LESS, GL_LESS },
																	  { CompareMode::LESS_EQUAL, GL_LEQUAL },
																	  { CompareMode::NEVER, GL_NEVER },
																	  { CompareMode::NOT_EQUAL, GL_NOTEQUAL } };

OpenGLES2Context::StencilActionMap OpenGLES2Context::m_stencilActions = { { StencilAction::DECREMENT_SATURATE, GL_DECR },
																		  { StencilAction::DECREMENT_WRAP, GL_DECR_WRAP },
																		  { StencilAction::INCREMENT_SATURATE, GL_INCR },
																		  { StencilAction::INCREMENT_WRAP, GL_INCR_WRAP },
																		  { StencilAction::INVERT, GL_INVERT },
																		  { StencilAction::KEEP, GL_KEEP },
																		  { StencilAction::SET, GL_REPLACE },
																		  { StencilAction::ZERO, GL_ZERO } };

OpenGLES2Context::OpenGLES2Context()
{
#if (AWAY_PLATFORM == AWAY_PLATFORM_WINDOWS) && !defined(AWAY_PLUGIN_ANGLE)
	glewInit();
#endif

	glEnable(GL_DEPTH_TEST); // Enable the depth test
	glEnable(GL_BLEND); // Enable blending
	glEnable(GL_CULL_FACE); // Discard polygons whose vertex winding order is opposite of the specified frontfacing mode
	glCullFace(GL_BACK);
	glFrontFace(GL_CW); // clockwise winding order is front

	m_curProgram = nullptr;
	m_curTriangleFaceToCull = TriangleFace::BACK;
	m_curFillMode = FillMode::SOLID;
	m_curSourceFactor = BlendFactor::ONE;
	m_curDestinationFactor = BlendFactor::ZERO;
	m_curDepthMask = true;
	m_renderToTexture = false;
	m_curDepthCompareMode = CompareMode::LESS;

	const char* glVendor = (const char*)glGetString(GL_VENDOR);
	const char* glRenderer = (const char*)glGetString(GL_RENDERER);
	const char* glVersion = (const char*)glGetString(GL_VERSION);
	m_driverInfo += glVendor ? glVendor : "(unknown vendor)";
	m_driverInfo += glRenderer ? glRenderer : "(unknown renderer)";
	m_driverInfo += glVersion ? glVersion : "(unknown version)";

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	m_viewportX = viewport[0];
	m_viewportY = viewport[1];
	m_viewportWidth = viewport[2];
	m_viewportHeight = viewport[3];
}

OpenGLES2Context::~OpenGLES2Context()
{

}

void OpenGLES2Context::clear(float red, float green, float blue, float alpha, float depth, unsigned int stencil, ClearMask mask)
{
	glClearColor(red, green, blue, alpha);

#ifdef GL_ES_VERSION_2_0
	glClearDepthf(depth);
#else
	glClearDepth(depth);
#endif

#ifndef AWAY_NO_STENCIL
	glClearStencil(stencil);
#endif

	GLbitfield internalMask = 0;
	int maskValue = static_cast<int>(mask);
	if (maskValue & static_cast<int>(ClearMask::COLOR))
		internalMask |= GL_COLOR_BUFFER_BIT;

	if (maskValue & static_cast<int>(ClearMask::DEPTH))
	{
		internalMask |= GL_DEPTH_BUFFER_BIT;
		glDepthMask(m_curDepthMask = true);
	}

#ifndef AWAY_NO_STENCIL
	if (maskValue & static_cast<int>(ClearMask::STENCIL))
		internalMask |= GL_STENCIL_BUFFER_BIT;
#endif

	glClear(internalMask);
}

void OpenGLES2Context::configureViewport(int x, int y, int width, int height, bool enableStencil)
{
	if (x != m_viewportX || y != m_viewportY || width != m_viewportWidth || height != m_viewportHeight)
	{
		m_viewportX = x;
		m_viewportY = y;
		m_viewportWidth = width;
		m_viewportHeight = height;

		glViewport(x, y, width, height);
	}

	if (enableStencil)
		glEnable(GL_STENCIL_TEST);
	else
		glDisable(GL_STENCIL_TEST);
}

Texture* OpenGLES2Context::createTexture(int width, int height, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels)
{
	return new Texture(width, height, format, optimizeForRenderToTexture, streamingLevels);
}

CubeTexture* OpenGLES2Context::createCubeTexture(int size, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels)
{
	return new CubeTexture(size, format, optimizeForRenderToTexture, streamingLevels);
}

IndexBuffer* OpenGLES2Context::createIndexBuffer(int numIndices, BufferUsage bufferUsage)
{
	return new IndexBuffer(numIndices, bufferUsage);
}

VertexBuffer* OpenGLES2Context::createVertexBuffer(int numVertices, int bytesPerVertex, BufferUsage bufferUsage)
{
	return new VertexBuffer(numVertices, bytesPerVertex, bufferUsage);
}

Program* OpenGLES2Context::createProgram()
{
	return new Program();
}

void OpenGLES2Context::drawLines(IndexBuffer* indexBuffer, int firstIndex, int numLines)
{
	if (indexBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getId());
		glDrawElements(GL_LINES, numLines * 2, GL_UNSIGNED_SHORT, (const GLvoid*)firstIndex);
	}
	else
	{
		glDrawArrays(GL_LINES, firstIndex, numLines * 2);
	}
}

void OpenGLES2Context::drawTriangles(IndexBuffer* indexBuffer, int firstIndex, int numTriangles)
{
	char name[8];
	GLenum index = 0;
	for (int i = 0; i < 8; i++)
	{
		TextureState& textureState = m_textureStates[i];
		TextureBase* texture = textureState.m_texture;
		if (texture)
		{
			// bind texture to texture unit
			GLenum target = (texture->getType() == TextureType::TWO_D ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP);
			glActiveTexture(GL_TEXTURE0 + index);
			glBindTexture(target, texture->getId());

			// set sampler uniform
#if _MSC_VER >= 1400
			sprintf_s(name, 8, "fs%d", i);
#else
			snprintf(name, 8, "fs%d", i);
#endif
			glUniform1i(m_curProgram->getUniformLocation(name), index++);

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
			if (texture->getFormat() == TextureFormat::COMPRESSED_ALPHA)
			{
				glActiveTexture(GL_TEXTURE0 + index);
				glBindTexture(target, texture->getAlphaId());

				snprintf(name, 8, "fw%d", i);
				glUniform1i(m_curProgram->getUniformLocation(name), index++);
			}
#endif

			// set texture parameters
			glTexParameteri(target, GL_TEXTURE_WRAP_S, textureState.m_wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, textureState.m_wrap);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, textureState.m_magFilter);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, textureState.m_minfilter);
		}
	}

	if (indexBuffer)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getId());
		glDrawElements(GL_TRIANGLES, numTriangles * 3, GL_UNSIGNED_SHORT, (const GLvoid*)firstIndex);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, firstIndex, numTriangles * 3);
	}
}

void OpenGLES2Context::present()
{

}

void OpenGLES2Context::setBlendFactors(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
	if (sourceFactor != m_curSourceFactor || destinationFactor != m_curDestinationFactor)
	{
		glBlendFunc(m_blendFactors[sourceFactor], m_blendFactors[destinationFactor]);
		m_curSourceFactor = sourceFactor;
		m_curDestinationFactor = destinationFactor;
	}
}

void OpenGLES2Context::setColorMask(bool red, bool green, bool blue, bool alpha)
{
	glColorMask(red, green, blue, alpha);
}

void OpenGLES2Context::setCulling(TriangleFace triangleFaceToCull)
{
	if (triangleFaceToCull != m_curTriangleFaceToCull)
	{
		if (m_curTriangleFaceToCull == TriangleFace::NONE)
			glEnable(GL_CULL_FACE);

		if (triangleFaceToCull == TriangleFace::NONE)
			glDisable(GL_CULL_FACE);
		else
			glCullFace(m_triangleFaces[triangleFaceToCull]);

		m_curTriangleFaceToCull = triangleFaceToCull;
	}
}

void OpenGLES2Context::setDepthTest(bool depthMask, CompareMode passCompareMode)
{
	if (depthMask != m_curDepthMask)
	{
		glDepthMask(depthMask);
		m_curDepthMask = depthMask;
	}

	if (passCompareMode != m_curDepthCompareMode)
	{
		glDepthFunc(m_compareFuncs[passCompareMode]);
		m_curDepthCompareMode = passCompareMode;
	}
}

void OpenGLES2Context::setFillMode(FillMode fillMode)
{
#ifndef GL_ES_VERSION_2_0
	if (fillMode != m_curFillMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, fillMode == FillMode::SOLID ? GL_FILL : GL_LINE);
		m_curFillMode = fillMode;
	}
#endif
}

void OpenGLES2Context::setProgram(Program* program)
{
	if (program != m_curProgram)
	{
		glUseProgram(program->getId());
		m_curProgram = program;
		glUniform1f(m_curProgram->getUniformLocation("yflip"), m_renderToTexture ? -1.f : 1.f);
	}
}

void OpenGLES2Context::setProgramConstantsFromMatrix(ProgramType programType, int firstRegister, Matrix3D& matrix, bool transposedMatrix)
{
	float(&raw)[16] = matrix.m_rawData;
	if (programType == ProgramType::VERTEX && m_curProgram->hasVertexIndirect())
	{
		int location = m_curProgram->getUniformLocation("vcarrr[0]");
		if (location >= 0)
		{
			location += firstRegister;
			if (transposedMatrix)
			{
				glUniform4f(location, raw[0], raw[4], raw[8], raw[12]);
				glUniform4f(location + 1, raw[1], raw[5], raw[9], raw[13]);
				glUniform4f(location + 2, raw[2], raw[6], raw[10], raw[14]);
				glUniform4f(location + 3, raw[3], raw[7], raw[11], raw[15]);
			}
			else
			{
				glUniform4fv(location, 4, raw);
			}
		}
	}
	else
	{
		char name[8];
		const char* format = programType == ProgramType::VERTEX ? "vc%d" : "fc%d";
		for (int i = 0, j; i < 4; i++)
		{
#if _MSC_VER >= 1400
			sprintf_s(name, 8, format, firstRegister + i);
#else
			snprintf(name, 8, format, firstRegister + i);
#endif
			int location = m_curProgram->getUniformLocation(name);
			if (location >= 0)
			{
				if (transposedMatrix)
				{
					glUniform4f(location, raw[i], raw[i + 4], raw[i + 8], raw[i + 12]);
				}
				else
				{
					j = i * 4;
					glUniform4f(location, raw[j], raw[j + 1], raw[j + 2], raw[j + 3]);
				}
			}
		}
	}
}

void OpenGLES2Context::setProgramConstantsFromVector(ProgramType programType, int firstRegister, float* data, int numRegisters)
{
	if (programType == ProgramType::VERTEX && m_curProgram->hasVertexIndirect())
	{
		int location = m_curProgram->getUniformLocation("vcarrr[0]");
		if (location >= 0)
		{
			glUniform4fv(location + firstRegister, numRegisters, data);
		}
	}
	else
	{
		char name[8];
		const char* format = programType == ProgramType::VERTEX ? "vc%d" : "fc%d";
		for (int i = 0, j; i < numRegisters; i++)
		{
#if _MSC_VER >= 1400
			sprintf_s(name, 8, format, firstRegister + i);
#else
			snprintf(name, 8, format, firstRegister + i);
#endif
			int location = m_curProgram->getUniformLocation(name);
			if (location >= 0)
			{
				j = i * 4;
				glUniform4f(location, data[j], data[j + 1], data[j + 2], data[j + 3]);
			}
		}
	}
}

void OpenGLES2Context::setRenderToBackBuffer()
{
	m_renderToTexture = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(m_viewportX, m_viewportY, m_viewportWidth, m_viewportHeight);
}

void OpenGLES2Context::setRenderToTexture(TextureBase* texture, bool enableDepthAndStencil, int antiAlias, int surfaceSelector, int colorOutputIndex)
{
	m_renderToTexture = true;
	glBindFramebuffer(GL_FRAMEBUFFER, texture->getFrameBuffer());
	if (texture->getType() == TextureType::Cube)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + surfaceSelector, texture->getId(), 0);
	}
	glViewport(0, 0, texture->getWidth(), texture->getHeight());
}

void OpenGLES2Context::setSamplerStateAt(int sampler, WrapMode wrap, TextureFilter filter, MipFilter mipfilter)
{
	TextureState& textureState = m_textureStates[sampler];
	switch (wrap)
	{
	case WrapMode::CLAMP:
		textureState.m_wrap = GL_CLAMP_TO_EDGE;
		break;
	case WrapMode::REPEAT:
		textureState.m_wrap = GL_REPEAT;
		break;
	}

	switch (filter)
	{
	case TextureFilter::LINEAR:
		textureState.m_magFilter = GL_LINEAR;
		switch (mipfilter)
		{
		case MipFilter::MIPLINEAR:
			textureState.m_minfilter = GL_LINEAR_MIPMAP_LINEAR;
			break;
		case MipFilter::MIPNEAREST:
			textureState.m_minfilter = GL_LINEAR_MIPMAP_NEAREST;
			break;
		case MipFilter::MIPNONE:
			textureState.m_minfilter = GL_LINEAR;
			break;
		}
		break;
	case TextureFilter::NEAREST:
		textureState.m_magFilter = GL_NEAREST;
		switch (mipfilter)
		{
		case MipFilter::MIPLINEAR:
			textureState.m_minfilter = GL_NEAREST_MIPMAP_LINEAR;
			break;
		case MipFilter::MIPNEAREST:
			textureState.m_minfilter = GL_NEAREST_MIPMAP_NEAREST;
			break;
		case MipFilter::MIPNONE:
			textureState.m_minfilter = GL_NEAREST;
			break;
		}
		break;
	}
}

void OpenGLES2Context::setScissorRectangle(Rectangle* rectangle)
{
	if (rectangle)
	{
		glEnable(GL_SCISSOR_TEST);

		GLint x, y;
		GLsizei width, height;
		if (rectangle->m_width < 0 || rectangle->m_height < 0)
		{
			x = m_viewportX;
			y = m_viewportY;
			width = m_viewportWidth;
			height = m_viewportHeight;
		}
		else
		{
			x = (GLint)rectangle->m_x;
			y = (GLint)rectangle->m_y;
			width = (GLsizei)rectangle->m_width;
			height = (GLsizei)rectangle->m_height;
		}

		glScissor(x, y, width, height);
	}
	else
		glDisable(GL_SCISSOR_TEST);
}

void OpenGLES2Context::setStencilActions(TriangleFace triangleFace, CompareMode compareMode, StencilAction actionOnBothPass, StencilAction actionOnDepthFail, StencilAction actionOnDepthPassStencilFail)
{
	if (compareMode != m_curStencilFunc)
	{
		m_curStencilFunc = compareMode;
		glStencilFuncSeparate(m_triangleFaces[triangleFace], m_compareFuncs[compareMode], m_curStencilRef, m_curStencilMask);
	}

	if (actionOnBothPass != m_curStencilBothPassOp || actionOnDepthFail != m_curStencilDepthFailOp || actionOnDepthPassStencilFail != m_curStencilDepthPassStencilFailOp)
	{
		m_curStencilBothPassOp = actionOnBothPass;
		m_curStencilDepthFailOp = actionOnDepthFail;
		m_curStencilDepthPassStencilFailOp = actionOnDepthPassStencilFail;
		glStencilOpSeparate(m_triangleFaces[triangleFace], m_stencilActions[actionOnDepthFail], m_stencilActions[actionOnDepthPassStencilFail], m_stencilActions[actionOnBothPass]);
	}
}

void OpenGLES2Context::setStencilReferenceValue(int referenceValue, unsigned int readMask, unsigned int writeMask)
{
	if (referenceValue != m_curStencilRef || readMask != m_curStencilMask)
	{
		glStencilFunc(m_compareFuncs[m_curStencilFunc], referenceValue, readMask);
		m_curStencilRef = referenceValue;
		m_curStencilMask = readMask;
	}
}

void OpenGLES2Context::setTextureAt(int sampler, TextureBase* texture)
{
	m_textureStates[sampler].m_texture = texture;
}

void OpenGLES2Context::setVertexBufferAt(int index, VertexBuffer* buffer, int offset, VertexBufferFormat format)
{
	if (!m_curProgram)
		return;

	char name[8];
#if _MSC_VER >= 1400
	sprintf_s(name, 8, "va%d", index);
#else
	snprintf(name, 8, "va%d", index);
#endif
	GLint location = m_curProgram->getAttributeLocation(name);
	if (location > -1)
	{
		GLint size;
		GLenum type;
		switch (format)
		{
		case VertexBufferFormat::BYTES_1:
			size = 1;
			type = GL_UNSIGNED_BYTE;
			break;
		case VertexBufferFormat::BYTES_2:
			size = 2;
			type = GL_UNSIGNED_BYTE;
			break;
		case VertexBufferFormat::BYTES_3:
			size = 3;
			type = GL_UNSIGNED_BYTE;
			break;
		case VertexBufferFormat::BYTES_4:
			size = 4;
			type = GL_UNSIGNED_BYTE;
			break;
		case VertexBufferFormat::FLOAT_1:
			size = 1;
			type = GL_FLOAT;
			break;
		case VertexBufferFormat::FLOAT_2:
			size = 2;
			type = GL_FLOAT;
			break;
		case VertexBufferFormat::FLOAT_3:
			size = 3;
			type = GL_FLOAT;
			break;
		case VertexBufferFormat::FLOAT_4:
			size = 4;
			type = GL_FLOAT;
			break;
		}

		if (buffer)
		{
			glEnableVertexAttribArray(location);
			glBindBuffer(GL_ARRAY_BUFFER, buffer->getId());
			glVertexAttribPointer(location, size, type, GL_FALSE, buffer->getBytesPerVertex(), (GLvoid*)offset);
		}
		else
		{
			glDisableVertexAttribArray(location);
		}
	}
}