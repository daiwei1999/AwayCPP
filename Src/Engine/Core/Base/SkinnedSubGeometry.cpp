#include "SkinnedSubGeometry.h"
#include "IContext.h"
#include "VertexBuffer.h"

USING_AWAY_NAMESPACE

SkinnedSubGeometry::SkinnedSubGeometry(unsigned char jointsPerVertex)
{
	m_jointsPerVertex = jointsPerVertex;
	switch (jointsPerVertex)
	{
	case 1:
		m_jointIndexFormat = VertexBufferFormat::BYTES_1;
		m_jointWeightFormat = VertexBufferFormat::FLOAT_1;
		break;
	case 2:
		m_jointIndexFormat = VertexBufferFormat::BYTES_2;
		m_jointWeightFormat = VertexBufferFormat::FLOAT_2;
		break;
	case 3:
		m_jointIndexFormat = VertexBufferFormat::BYTES_3;
		m_jointWeightFormat = VertexBufferFormat::FLOAT_3;
		break;
	case 4:
		m_jointIndexFormat = VertexBufferFormat::BYTES_4;
		m_jointWeightFormat = VertexBufferFormat::FLOAT_4;
		break;
	}
}

void SkinnedSubGeometry::activateJointIndexBuffer(int index, IContext* context)
{
	if (m_jointIndexContext != context || !m_jointIndexBuffer)
	{
		m_jointIndexBuffer = context->createVertexBuffer(m_numVertices, m_jointsPerVertex);
		m_jointIndexContext = context;
		m_jointIndicesInvalid = true;
	}
	if (m_jointIndicesInvalid)
	{
		m_jointIndexBuffer->uploadFromVector(m_jointIndexData, 0, m_numVertices);
		m_jointIndicesInvalid = false;
	}
	context->setVertexBufferAt(index, m_jointIndexBuffer, 0, m_jointIndexFormat);
}

void SkinnedSubGeometry::activateJointWeightsBuffer(int index, IContext* context)
{
	if (m_jointWeightContext != context || !m_jointWeightsBuffer)
	{
		m_jointWeightsBuffer = context->createVertexBuffer(m_numVertices, m_jointsPerVertex * 4);
		m_jointWeightContext = context;
		m_jointWeightsInvalid = true;
	}
	if (m_jointWeightsInvalid)
	{
		m_jointWeightsBuffer->uploadFromVector(m_jointWeightsData, 0, m_numVertices);
		m_jointWeightsInvalid = false;
	}
	context->setVertexBufferAt(index, m_jointWeightsBuffer, 0, m_jointWeightFormat);
}

void SkinnedSubGeometry::updateJointIndexData(unsigned char* data)
{
	m_jointIndexData = data;
	m_jointIndicesInvalid = true;
}

void SkinnedSubGeometry::updateJointWeightsData(float* data)
{
	m_jointWeightsData = data;
	m_jointWeightsInvalid = true;
}

void SkinnedSubGeometry::updateAnimatedData(float* data)
{
	m_animatedData = data;
	m_activeDataInvalid = true;
}

void SkinnedSubGeometry::uploadData()
{
	if (m_animatedData)
	{
		m_activeBuffer->uploadFromVector(m_animatedData, 0, m_numVertices);
		m_activeDataInvalid = false;
	}
	else
	{
		CompactSubGeometry::uploadData();
	}
}