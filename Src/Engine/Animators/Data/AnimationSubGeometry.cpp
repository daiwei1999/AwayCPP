#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "VertexBuffer.h"

USING_AWAY_NAMESPACE

AnimationSubGeometry::AnimationSubGeometry()
{
	m_numProcessedVertices = 0;
	m_vertexBuffer = nullptr;
	m_bufferContext = nullptr;
	m_bufferDirty = true;
}

void AnimationSubGeometry::createVertexData(int numVertices, int totalLenOfOneVertex)
{
	m_numVertices = numVertices;
	m_totalLenOfOneVertex = totalLenOfOneVertex;
	m_vertexData = new float[numVertices * totalLenOfOneVertex];
}

void AnimationSubGeometry::activateVertexBuffer(int index, int bufferOffset, IContext* context, VertexBufferFormat format)
{
	if (!m_vertexBuffer || m_bufferContext != context)
	{
		m_vertexBuffer = context->createVertexBuffer(m_numVertices, m_totalLenOfOneVertex * 4);
		m_bufferContext = context;
		m_bufferDirty = true;
	}
	if (m_bufferDirty)
	{
		m_vertexBuffer->uploadFromVector(m_vertexData, 0, m_numVertices);
		m_bufferDirty = false;
	}

	context->setVertexBufferAt(index, m_vertexBuffer, bufferOffset * 4, format);
}

void AnimationSubGeometry::invalidateBuffer()
{
	m_bufferDirty = true;
}