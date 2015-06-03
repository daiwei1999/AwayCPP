#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class IndexBuffer
{
public:
	IndexBuffer(int numIndices, BufferUsage bufferUsage);
	~IndexBuffer();

	unsigned int getId() { return m_indexBuffer; }
	void uploadFromVector(unsigned short* data, int startOffset, int count);

private:
	unsigned int m_indexBuffer;
};

AWAY_NAMESPACE_END