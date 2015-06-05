#pragma once 

#include "Common.h"
#include "ISubGeometry.h"

AWAY_NAMESPACE_BEGIN

class SubGeometryBase : public ISubGeometry
{
public:
	SubGeometryBase();

	bool getAutoDeriveVertexNormals() { return m_autoDeriveVertexNormals; }
	void setAutoDeriveVertexNormals(bool value);
	bool getAutoDeriveVertexTangents() { return m_autoDeriveVertexTangents; }
	void setAutoDeriveVertexTangents(bool value);
	int getVertexCount() override { return m_numVertices; }
	int getTriangleCount() override { return m_numIndices / 3; }
	IndexBuffer* getIndexBuffer(IContext* context) override;

	unsigned short* getIndexData() override { return m_indices; }
	void updateIndexData(unsigned short* indices, int numIndices);
	
	Geometry* getParentGeometry() override { return m_parentGeometry; }
	void setParentGeometry(Geometry* value) override { m_parentGeometry = value; }

protected:
	void invalidateBounds();
	virtual void updateVertexNormals(float* target);
	virtual void updateVertexTangents(float* target);

private:
	void updateFaceNormals();
	void updateFaceTangents();

protected:
	float* m_vertexData;
	int m_numVertices;
	unsigned short* m_indices;
	int m_numIndices;
	IndexBuffer* m_indexBuffer;
	IContext* m_indexBufferContext;
	bool m_indicesInvalid;

	bool m_faceNormalsDirty;
	bool m_faceTangentsDirty;
	bool m_autoDeriveVertexNormals;
	bool m_autoDeriveVertexTangents;
	bool m_vertexNormalsDirty;
	bool m_vertexTangentsDirty;

private:
	float* m_faceNormals;
	float* m_faceTangents;
	Geometry* m_parentGeometry;
};

AWAY_NAMESPACE_END