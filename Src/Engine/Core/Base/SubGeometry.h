#pragma once

#include "Common.h"
#include "SubGeometryBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The SubGeometry class is a collections of geometric data that describes a triangle mesh. It is owned by a
 * Geometry instance, and wrapped by a SubMesh in the scene graph.
 * Several SubGeometries are grouped so they can be rendered with different materials, but still represent a single
 * object.
 */
class SubGeometry : public SubGeometryBase
{
public:
	SubGeometry();

	int getVertexStride() override { return 3; }
	int getVertexNormalStride() override { return 3; }
	int getVertexTangentStride() override { return 3; }
	int getUVStride() override { return 2; }
	int getSecondaryUVStride() override { return 2; }

	void activateVertexBuffer(int index, IContext* context) override;
	void activateUVBuffer(int index, IContext* context) override;
	void activateSecondaryUVBuffer(int index, IContext* context) override;
	void activateVertexNormalBuffer(int index, IContext* context) override;
	void activateVertexTangentBuffer(int index, IContext* context) override;

	float* getVertexData() override { return m_vertexData; }
	float* getVertexNormalData() override { return m_vertexNormals; }
	float* getVertexTangentData() override { return m_vertexTangents; }
	float* getUVData() override { return m_uvs; }
	float* getSecondaryUVData() override { return m_secondaryUVs; }
	int getVertexOffset() override { return 0; }
	int getVertexNormalOffset() override { return 0; }
	int getVertexTangentOffset() override { return 0; }
	int getUVOffset() override { return 0; }
	int getSecondaryUVOffset() override { return 0; }

	void updateVertexData(float* vertices, int numVertices);
	void updateVertexNormalData(float* vertexNormals, int numVertices);
	void updateVertexTangentData(float* vertexTangents, int numVertices);
	void updateUVData(float* uvs, int numVertices);
	void updateSecondaryUVData(float* uvs, int numVertices);
	void fromVectors(float* vertices, float* normals, float* tangents, float* uvs, float* secondaryUVs, int numVertices);
	
protected:
	float* m_uvs;
	float* m_secondaryUVs;
	float* m_vertexNormals;
	float* m_vertexTangents;
	bool m_verticesInvalid;
	bool m_uvsInvalid;
	bool m_secondaryUVsInvalid;
	bool m_normalsInvalid;
	bool m_tangentsInvalid;
	VertexBuffer* m_vertexBuffer;
	VertexBuffer* m_uvBuffer;
	VertexBuffer* m_secondaryUVBuffer;
	VertexBuffer* m_vertexNormalBuffer;
	VertexBuffer* m_vertexTangentBuffer;
	IContext* m_vertexBufferContext;
	IContext* m_uvBufferContext;
	IContext* m_secondaryUVBufferContext;
	IContext* m_vertexNormalBufferContext;
	IContext* m_vertexTangentBufferContext;
};

AWAY_NAMESPACE_END