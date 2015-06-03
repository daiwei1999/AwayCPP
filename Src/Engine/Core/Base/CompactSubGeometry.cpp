#include "CompactSubGeometry.h"
#include "IContext.h"
#include "VertexBuffer.h"

USING_AWAY_NAMESPACE

CompactSubGeometry::CompactSubGeometry()
{
	m_activeBuffer = nullptr;
	m_activeContext = nullptr;
	m_activeDataInvalid = false;
	m_autoDeriveVertexNormals = false;
	m_autoDeriveVertexTangents = false;
}

void CompactSubGeometry::activateVertexBuffer(int index, IContext* context)
{
	if (!m_activeBuffer || m_activeContext != context)
		createBuffer(context);

	if (m_activeDataInvalid)
		uploadData();

	context->setVertexBufferAt(index, m_activeBuffer, 0, VertexBufferFormat::FLOAT_3);
}

void CompactSubGeometry::activateVertexNormalBuffer(int index, IContext* context)
{
	if (!m_activeBuffer || m_activeContext != context)
		createBuffer(context);

	if (m_activeDataInvalid)
		uploadData();

	context->setVertexBufferAt(index, m_activeBuffer, 12, VertexBufferFormat::FLOAT_3);
}

void CompactSubGeometry::activateVertexTangentBuffer(int index, IContext* context)
{
	if (!m_activeBuffer || m_activeContext != context)
		createBuffer(context);

	if (m_activeDataInvalid)
		uploadData();

	context->setVertexBufferAt(index, m_activeBuffer, 24, VertexBufferFormat::FLOAT_3);
}

void CompactSubGeometry::activateUVBuffer(int index, IContext* context)
{
	if (!m_activeBuffer || m_activeContext != context)
		createBuffer(context);

	if (m_activeDataInvalid)
		uploadData();

	context->setVertexBufferAt(index, m_activeBuffer, 36, VertexBufferFormat::FLOAT_2);
}

void CompactSubGeometry::activateSecondaryUVBuffer(int index, IContext* context)
{
	if (!m_activeBuffer || m_activeContext != context)
		createBuffer(context);

	if (m_activeDataInvalid)
		uploadData();

	context->setVertexBufferAt(index, m_activeBuffer, 44, VertexBufferFormat::FLOAT_2);
}

float* CompactSubGeometry::getVertexData()
{
	if (m_autoDeriveVertexNormals && m_vertexNormalsDirty)
		updateVertexNormals(m_vertexData);

	if (m_autoDeriveVertexTangents && m_vertexTangentsDirty)
		updateVertexTangents(m_vertexData);

	return m_vertexData;
}

void CompactSubGeometry::updateData(float* data, int numVertices)
{
	if (m_autoDeriveVertexNormals)
		m_vertexNormalsDirty = true;

	if (m_autoDeriveVertexTangents)
		m_vertexTangentsDirty = true;

	m_faceNormalsDirty = true;
	m_faceTangentsDirty = true;

	m_vertexData = data;
	m_activeDataInvalid = true;

	if (numVertices != m_numVertices)
	{
		SAFE_DELETE(m_activeBuffer)
		m_numVertices = numVertices;
	}

	invalidateBounds();
}

void CompactSubGeometry::fromVectors(float* vertices, float* normals, float* tangents, float* uvs, float* secondaryUVs, int numVertices)
{
	int vertLen = numVertices * 13;
	float* data = new float[vertLen];
	int index = 0, v = 0, n = 0, t = 0, u = 0, s = 0;
	while (index < vertLen)
	{
		data[index++] = vertices[v++];
		data[index++] = vertices[v++];
		data[index++] = vertices[v++];

		if (normals)
		{
			data[index++] = normals[n++];
			data[index++] = normals[n++];
			data[index++] = normals[n++];
		}
		else
		{
			data[index++] = 0;
			data[index++] = 0;
			data[index++] = 0;
		}
		
		if (tangents)
		{
			data[index++] = tangents[t++];
			data[index++] = tangents[t++];
			data[index++] = tangents[t++];
		}
		else
		{
			data[index++] = 0;
			data[index++] = 0;
			data[index++] = 0;
		}
		
		if (uvs)
		{
			data[index++] = uvs[u++];
			data[index++] = uvs[u++];
		}
		else
		{
			data[index++] = 0;
			data[index++] = 0;
		}
		
		if (secondaryUVs)
		{
			data[index++] = secondaryUVs[s++];
			data[index++] = secondaryUVs[s++];
		}
		else
		{
			data[index++] = 0;
			data[index++] = 0;
		}
	}

	setAutoDeriveVertexNormals(normals ? false : true);
	setAutoDeriveVertexTangents(tangents ? false : true);
	updateData(data, numVertices);
}

void CompactSubGeometry::uploadData()
{
	m_activeBuffer->uploadFromVector(getVertexData(), 0, m_numVertices);
	m_activeDataInvalid = false;
}

void CompactSubGeometry::createBuffer(IContext* context)
{
	m_activeBuffer = context->createVertexBuffer(m_numVertices, 52);
	m_activeContext = context;
	m_activeDataInvalid = true;
}

void CompactSubGeometry::updateVertexNormals(float* target)
{
	m_activeDataInvalid = true;
	SubGeometryBase::updateVertexNormals(target);
}

void CompactSubGeometry::updateVertexTangents(float* target)
{
	if (m_vertexNormalsDirty)
		updateVertexNormals(m_vertexData);

	m_activeDataInvalid = true;
	SubGeometryBase::updateVertexTangents(target);
}