#pragma once

#include "Common.h"
#include "SubGeometryBase.h"

AWAY_NAMESPACE_BEGIN

class CompactSubGeometry : public SubGeometryBase
{
public:
	CompactSubGeometry();

	int getVertexStride() override { return 13; }
	int getVertexNormalStride() override { return 13; }
	int getVertexTangentStride() override { return 13; }
	int getUVStride() override { return 13; }
	int getSecondaryUVStride() override { return 13; }

	void activateVertexBuffer(int index, IContext* context) override;
	void activateVertexNormalBuffer(int index, IContext* context) override;
	void activateVertexTangentBuffer(int index, IContext* context) override;
	void activateUVBuffer(int index, IContext* context) override;
	void activateSecondaryUVBuffer(int index, IContext* context) override;

	float* getVertexData() override;
	float* getVertexNormalData() override { return m_vertexData; }
	float* getVertexTangentData() override { return m_vertexData; }
	float* getUVData() override { return m_vertexData; }
	float* getSecondaryUVData() override { return m_vertexData; }
	int getVertexOffset() override { return 0; }
	int getVertexNormalOffset() override { return 3; }
	int getVertexTangentOffset() override { return 6; }
	int getUVOffset() override { return 9; }
	int getSecondaryUVOffset() override { return 11; }

	void updateData(float* data, int numVertices);
	void fromVectors(float* vertices, float* normals, float* tangents, float* uvs, float* secondaryUVs, int numVertices);

protected:
	virtual void uploadData();
	void createBuffer(IContext* context);
	void updateVertexNormals(float* target) override;
	void updateVertexTangents(float* target) override;

protected:
	VertexBuffer* m_activeBuffer;
	IContext* m_activeContext;
	bool m_activeDataInvalid;
};

AWAY_NAMESPACE_END