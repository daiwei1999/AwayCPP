#include "IndexBuffer.h"
#include "IContext.h"

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

IndexBuffer::IndexBuffer(int numIndices, BufferUsage bufferUsage)
{
	glGenBuffers(1, &m_indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * 2, nullptr, bufferUsage == BufferUsage::STATIC_DRAW ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_indexBuffer);
}

void IndexBuffer::uploadFromVector(unsigned short* data, int startOffset, int count)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, startOffset * 2, count * 2, data);
}