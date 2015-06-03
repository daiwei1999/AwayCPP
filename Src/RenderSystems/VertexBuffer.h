#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class VertexBuffer
{
public:
	VertexBuffer(int numVertices, int bytesPerVertex, BufferUsage bufferUsage);
	~VertexBuffer();

	unsigned int getId() { return m_vertexBuffer; }
	int getBytesPerVertex() { return m_bytesPerVertex; }
	void uploadFromVector(void* data, int startVertex, int numVertices);

private:
	int m_bytesPerVertex;
	unsigned int m_vertexBuffer;
};

AWAY_NAMESPACE_END