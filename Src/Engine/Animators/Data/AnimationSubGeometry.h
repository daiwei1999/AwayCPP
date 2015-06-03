#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class AnimationSubGeometry
{
public:
	AnimationSubGeometry();

	float* getVertexData() { return m_vertexData; }
	int getVertexCount() { return m_numVertices; }
	int getTotalLenOfOneVertex() { return m_totalLenOfOneVertex; }
	void createVertexData(int numVertices, int totalLenOfOneVertex);
	void activateVertexBuffer(int index, int bufferOffset, IContext* context, VertexBufferFormat format);
	void invalidateBuffer();

public:
	float m_previousTime;
	int m_numProcessedVertices;
	std::vector<ParticleAnimationData*> m_animationParticles;

private:
	float* m_vertexData;
	VertexBuffer* m_vertexBuffer;
	IContext* m_bufferContext;
	bool m_bufferDirty;
	int m_numVertices;
	int m_totalLenOfOneVertex;
};

AWAY_NAMESPACE_END