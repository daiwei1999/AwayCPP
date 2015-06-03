#include "SubGeometryBase.h"
#include "IContext.h"
#include "IndexBuffer.h"
#include "Geometry.h"

USING_AWAY_NAMESPACE

SubGeometryBase::SubGeometryBase()
{
	m_vertexData = nullptr;
	m_numVertices = 0;
	m_indices = nullptr;
	m_numIndices = 0;
	m_indexBuffer = nullptr;
	m_indexBufferContext = nullptr;
	
	m_faceNormalsDirty = true;
	m_faceTangentsDirty = true;
	m_autoDeriveVertexNormals = true;
	m_autoDeriveVertexTangents = true;
	m_vertexNormalsDirty = true;
	m_vertexTangentsDirty = true;

	m_faceNormals = nullptr;
	m_faceTangents = nullptr;
	m_parentGeometry = nullptr;
}

void SubGeometryBase::setAutoDeriveVertexNormals(bool value)
{
	m_autoDeriveVertexNormals = value;
	m_vertexNormalsDirty = value;
}

void SubGeometryBase::setAutoDeriveVertexTangents(bool value)
{
	m_autoDeriveVertexTangents = value;
	m_vertexTangentsDirty = value;
}

IndexBuffer* SubGeometryBase::getIndexBuffer(IContext* context)
{
	if (!m_indexBuffer || m_indexBufferContext != context)
	{
		m_indexBuffer = context->createIndexBuffer(m_numIndices);
		m_indexBufferContext = context;
		m_indicesInvalid = true;
	}
	if (m_indicesInvalid)
	{
		m_indexBuffer->uploadFromVector(m_indices, 0, m_numIndices);
		m_indicesInvalid = false;
	}
	return m_indexBuffer;
}

void SubGeometryBase::updateIndexData(unsigned short* indices, int numIndices)
{
	m_indices = indices;
	m_indicesInvalid = true;
	m_faceNormalsDirty = true;

	if (numIndices != m_numIndices)
	{
		SAFE_DELETE(m_indexBuffer)
		m_numIndices = numIndices;
	}

	if (m_autoDeriveVertexNormals)
		m_vertexNormalsDirty = true;

	if (m_autoDeriveVertexTangents)
		m_vertexTangentsDirty = true;
}

void SubGeometryBase::invalidateBounds()
{
	if (m_parentGeometry)
		m_parentGeometry->invalidateBounds(this);
}

void SubGeometryBase::updateVertexNormals(float* target)
{
	if (m_faceNormalsDirty)
		updateFaceNormals();

	int normalStride = getVertexNormalStride();
	int normalOffset = getVertexNormalOffset();

	// reset vertex normal to 0
	int i = 0, index = normalOffset;
	for (i = 0; i < m_numVertices; i++)
	{
		target[index] = target[index + 1] = target[index + 2] = 0;
		index += normalStride;
	}

	int f1 = 0, f2 = 1, f3 = 2;
	for (i = 0; i < m_numIndices; i += 3)
	{
		index = m_indices[i] * normalStride + normalOffset;
		target[index] += m_faceNormals[f1];
		target[index + 1] += m_faceNormals[f2];
		target[index + 2] += m_faceNormals[f3];

		index = m_indices[i + 1] * normalStride + normalOffset;
		target[index] += m_faceNormals[f1];
		target[index + 1] += m_faceNormals[f2];
		target[index + 2] += m_faceNormals[f3];

		index = m_indices[i + 2] * normalStride + normalOffset;
		target[index] += m_faceNormals[f1];
		target[index + 1] += m_faceNormals[f2];
		target[index + 2] += m_faceNormals[f3];

		f1 += 3;
		f2 += 3;
		f3 += 3;
	}
	SAFE_DELETE_ARRAY(m_faceNormals)

	index = normalOffset;
	for (i = 0; i < m_numVertices; i++)
	{
		float vx = target[index];
		float vy = target[index + 1];
		float vz = target[index + 2];
		float d = 1 / std::sqrt(vx * vx + vy * vy + vz * vz);
		target[index] = vx * d;
		target[index + 1] = vy * d;
		target[index + 2] = vz * d;
		index += normalStride;
	}

	m_vertexNormalsDirty = false;
}

void SubGeometryBase::updateVertexTangents(float* target)
{
	if (m_faceTangentsDirty)
		updateFaceTangents();

	int tangentStride = getVertexTangentStride();
	int tangentOffset = getVertexTangentOffset();

	// reset vertex tangent to 0
	int i, index = tangentOffset;
	for (i = 0; i < m_numVertices; i++)
	{
		target[index] = target[index + 1] = target[index + 2] = 0;
		index += tangentStride;
	}

	int f1 = 0, f2 = 1, f3 = 2;
	for (i = 0; i < m_numIndices; i += 3)
	{
		index = m_indices[i] * tangentStride + tangentOffset;
		target[index++] += m_faceTangents[f1];
		target[index++] += m_faceTangents[f2];
		target[index] += m_faceTangents[f3];

		index = m_indices[i + 1] * tangentStride + tangentOffset;
		target[index++] += m_faceTangents[f1];
		target[index++] += m_faceTangents[f2];
		target[index] += m_faceTangents[f3];

		index = m_indices[i + 2] * tangentStride + tangentOffset;
		target[index++] += m_faceTangents[f1];
		target[index++] += m_faceTangents[f2];
		target[index] += m_faceTangents[f3];

		f1 += 3;
		f2 += 3;
		f3 += 3;
	}
	SAFE_DELETE_ARRAY(m_faceTangents)

	index = tangentOffset;
	for (i = 0; i < m_numVertices; i++)
	{
		float vx = target[index];
		float vy = target[index + 1];
		float vz = target[index + 2];
		float d = 1 / std::sqrt(vx * vx + vy * vy + vz * vz);
		target[index] = vx * d;
		target[index + 1] = vy * d;
		target[index + 2] = vz * d;
		index += tangentStride;
	}

	m_vertexTangentsDirty = false;
}

void SubGeometryBase::updateFaceNormals()
{
	int index;
	int posStride = getVertexStride();
	int posOffset = getVertexOffset();
	float x1, y1, z1, x2, y2, z2, x3, y3, z3, dx1, dy1, dz1, dx2, dy2, dz2, cx, cy, cz, d;

	if (!m_faceNormals)
		m_faceNormals = new float[m_numIndices];

	for (int i = 0; i < m_numIndices; i += 3)
	{
		index = m_indices[i] * posStride + posOffset;
		x1 = m_vertexData[index];
		y1 = m_vertexData[index + 1];
		z1 = m_vertexData[index + 2];

		index = m_indices[i + 1] * posStride + posOffset;
		x2 = m_vertexData[index];
		y2 = m_vertexData[index + 1];
		z2 = m_vertexData[index + 2];

		index = m_indices[i + 2] * posStride + posOffset;
		x3 = m_vertexData[index];
		y3 = m_vertexData[index + 1];
		z3 = m_vertexData[index + 2];

		dx1 = x3 - x1;
		dy1 = y3 - y1;
		dz1 = z3 - z1;
		dx2 = x2 - x1;
		dy2 = y2 - y1;
		dz2 = z2 - z1;
		cx = dz1 * dy2 - dy1 * dz2;
		cy = dx1 * dz2 - dz1 * dx2;
		cz = dy1 * dx2 - dx1 * dy2;
		d = 1 / std::sqrt(cx * cx + cy * cy + cz * cz);
		m_faceNormals[i] = cx * d;
		m_faceNormals[i + 1] = cy * d;
		m_faceNormals[i + 2] = cz * d;
	}

	m_faceNormalsDirty = false;
}

void SubGeometryBase::updateFaceTangents()
{
	int index1, index2, index3, ui, vi;
	float* uvs = getUVData();
	int posStride = getVertexStride();
	int posOffset = getVertexOffset();
	int uvStride = getUVStride();
	int uvOffset = getUVOffset();
	float v0, dv1, dv2, x0, y0, z0, dx1, dy1, dz1, dx2, dy2, dz2, cx, cy, cz, d;

	if (!m_faceTangents)
		m_faceTangents = new float[m_numIndices];

	for (int i = 0; i < m_numIndices; i += 3)
	{
		index1 = m_indices[i];
		index2 = m_indices[i + 1];
		index3 = m_indices[i + 2];

		ui = index1 * uvStride + uvOffset + 1;
		v0 = uvs[ui];
		ui = index2 * uvStride + uvOffset + 1;
		dv1 = uvs[ui] - v0;
		ui = index3 * uvStride + uvOffset + 1;
		dv2 = uvs[ui] - v0;

		vi = index1 * posStride + posOffset;
		x0 = m_vertexData[vi];
		y0 = m_vertexData[vi + 1];
		z0 = m_vertexData[vi + 2];
		vi = index2 * posStride + posOffset;
		dx1 = m_vertexData[vi] - x0;
		dy1 = m_vertexData[vi + 1] - y0;
		dz1 = m_vertexData[vi + 2] - z0;
		vi = index3 * posStride + posOffset;
		dx2 = m_vertexData[vi] - x0;
		dy2 = m_vertexData[vi + 1] - y0;
		dz2 = m_vertexData[vi + 2] - z0;

		cx = dv2 * dx1 - dv1 * dx2;
		cy = dv2 * dy1 - dv1 * dy2;
		cz = dv2 * dz1 - dv1 * dz2;
		d = 1 / std::sqrt(cx * cx + cy * cy + cz * cz);
		m_faceTangents[i] = d * cx;
		m_faceTangents[i + 1] = d * cy;
		m_faceTangents[i + 2] = d * cz;
	}

	m_faceTangentsDirty = false;
}