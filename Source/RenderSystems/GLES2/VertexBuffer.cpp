#include "VertexBuffer.h"
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

VertexBuffer::VertexBuffer(int numVertices, int bytesPerVertex, BufferUsage bufferUsage)
{
	m_bytesPerVertex = bytesPerVertex;
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * m_bytesPerVertex, nullptr, bufferUsage == BufferUsage::STATIC_DRAW ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_vertexBuffer);
}

void VertexBuffer::uploadFromVector(void* data, int startVertex, int numVertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, startVertex * m_bytesPerVertex, numVertices * m_bytesPerVertex, data);
}