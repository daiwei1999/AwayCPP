#include "SubGeometry.h"
#include "IContext.h"
#include "VertexBuffer.h"

USING_AWAY_NAMESPACE

SubGeometry::SubGeometry()
{
	m_uvs = nullptr;
	m_secondaryUVs = nullptr;
	m_vertexNormals = nullptr;
	m_vertexTangents = nullptr;

	m_vertexBuffer = nullptr;
	m_uvBuffer = nullptr;
	m_secondaryUVBuffer = nullptr;
	m_vertexNormalBuffer = nullptr;
	m_vertexTangentBuffer = nullptr;
	m_vertexBufferContext = nullptr;
	m_uvBufferContext = nullptr;
	m_secondaryUVBufferContext = nullptr;
	m_vertexNormalBufferContext = nullptr;
	m_vertexTangentBufferContext = nullptr;
}

void SubGeometry::activateVertexBuffer(int index, IContext* context)
{
	if (!m_vertexBuffer || m_vertexBufferContext != context)
	{
		m_vertexBuffer = context->createVertexBuffer(m_numVertices, 12);
		m_vertexBufferContext = context;
		m_verticesInvalid = true;
	}
	if (m_verticesInvalid)
	{
		m_vertexBuffer->uploadFromVector(m_vertexData, 0, m_numVertices);
		m_verticesInvalid = false;
	}

	context->setVertexBufferAt(index, m_vertexBuffer, 0, VertexBufferFormat::FLOAT_3);
}

void SubGeometry::activateUVBuffer(int index, IContext* context)
{
	if (!m_uvBuffer || m_uvBufferContext != context)
	{
		m_uvBuffer = context->createVertexBuffer(m_numVertices, 8);
		m_uvBufferContext = context;
		m_uvsInvalid = true;
	}
	if (m_uvsInvalid)
	{
		m_uvBuffer->uploadFromVector(m_uvs, 0, m_numVertices);
		m_uvsInvalid = false;
	}

	context->setVertexBufferAt(index, m_uvBuffer, 0, VertexBufferFormat::FLOAT_2);
}

void SubGeometry::activateSecondaryUVBuffer(int index, IContext* context)
{
	if (!m_secondaryUVBuffer || m_secondaryUVBufferContext != context)
	{
		m_secondaryUVBuffer = context->createVertexBuffer(m_numVertices, 8);
		m_secondaryUVBufferContext = context;
		m_secondaryUVsInvalid = true;
	}
	if (m_secondaryUVsInvalid)
	{
		m_secondaryUVBuffer->uploadFromVector(m_secondaryUVs, 0, m_numVertices);
		m_secondaryUVsInvalid = false;
	}

	context->setVertexBufferAt(index, m_secondaryUVBuffer, 0, VertexBufferFormat::FLOAT_2);
}

void SubGeometry::activateVertexNormalBuffer(int index, IContext* context)
{
	if (m_autoDeriveVertexNormals && m_vertexNormalsDirty)
		updateVertexNormals(m_vertexNormals);

	if (!m_vertexNormalBuffer || m_vertexNormalBufferContext != context)
	{
		m_vertexNormalBuffer = context->createVertexBuffer(m_numVertices, 12);
		m_vertexNormalBufferContext = context;
		m_normalsInvalid = true;
	}
	if (m_normalsInvalid)
	{
		m_vertexNormalBuffer->uploadFromVector(m_vertexNormals, 0, m_numVertices);
		m_normalsInvalid = false;
	}

	context->setVertexBufferAt(index, m_vertexNormalBuffer, 0, VertexBufferFormat::FLOAT_3);
}

void SubGeometry::activateVertexTangentBuffer(int index, IContext* context)
{
	if (m_vertexTangentsDirty)
		updateVertexTangents(m_vertexTangents);

	if (!m_vertexTangentBuffer || m_vertexTangentBufferContext != context)
	{
		m_vertexTangentBuffer = context->createVertexBuffer(m_numVertices, 12);
		m_vertexTangentBufferContext = context;
		m_tangentsInvalid = true;
	}
	if (m_tangentsInvalid)
	{
		m_vertexTangentBuffer->uploadFromVector(m_vertexTangents, 0, m_numVertices);
		m_tangentsInvalid = false;
	}

	context->setVertexBufferAt(index, m_vertexTangentBuffer, 0, VertexBufferFormat::FLOAT_3);
}

void SubGeometry::updateVertexData(float* vertices, int numVertices)
{
	if (m_autoDeriveVertexNormals)
		m_vertexNormalsDirty = true;

	if (m_autoDeriveVertexTangents)
		m_vertexTangentsDirty = true;

	m_faceNormalsDirty = true;
	m_vertexData = vertices;
	m_verticesInvalid = true;

	if (numVertices != m_numVertices)
	{
		SAFE_DELETE(m_vertexBuffer)
		SAFE_DELETE(m_vertexNormalBuffer)
		SAFE_DELETE(m_vertexTangentBuffer)
		SAFE_DELETE(m_uvBuffer)
		SAFE_DELETE(m_secondaryUVBuffer)
		m_numVertices = numVertices;
	}

	invalidateBounds();
}

void SubGeometry::updateVertexNormalData(float* vertexNormals, int numVertices)
{
	m_vertexNormalsDirty = false;
	m_autoDeriveVertexNormals = (vertexNormals ? false : true);
	m_vertexNormals = vertexNormals;
	m_normalsInvalid = true;
}

void SubGeometry::updateVertexTangentData(float* vertexTangents, int numVertices)
{
	m_vertexTangentsDirty = false;
	m_autoDeriveVertexTangents = (vertexTangents ? false : true);
	m_vertexTangents = vertexTangents;
	m_tangentsInvalid = true;
}

void SubGeometry::updateUVData(float* uvs, int numVertices)
{
	if (m_autoDeriveVertexTangents)
		m_vertexTangentsDirty = true;

	m_faceTangentsDirty = true;
	m_uvs = uvs;
	m_uvsInvalid = true;
}

void SubGeometry::updateSecondaryUVData(float* uvs, int numVertices)
{
	m_secondaryUVs = uvs;
	m_secondaryUVsInvalid = true;
}

void SubGeometry::fromVectors(float* vertices, float* normals, float* tangents, float* uvs, float* secondaryUVs, int numVertices)
{
	updateVertexData(vertices, numVertices);
	updateVertexNormalData(normals, numVertices);
	updateVertexTangentData(tangents, numVertices);
	updateUVData(uvs, numVertices);
	updateSecondaryUVData(secondaryUVs, numVertices);
}