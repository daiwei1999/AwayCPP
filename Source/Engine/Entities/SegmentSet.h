#pragma once

#include "Common.h"
#include "Entity.h"
#include "IRenderable.h"

AWAY_NAMESPACE_BEGIN

class SegmentSet : public Entity, public IRenderable
{
public:
	SegmentSet();

	void addSegment(Segment* segment);
	void removeSegment(Segment* segment);
	void removeAllSegments();

	Segment* getSegment(int index) { return m_segments[index]; }
	void updateSegment(Segment* segment);
	bool hasData() { return m_segments.size() > 0; }

	Matrix3D& getRenderSceneTransform(Camera3D* camera) override { return m_sceneTransform; }
	Matrix3D& getInverseSceneTransform() override { return Entity::getInverseSceneTransform(); }
	bool getMouseEnabled() override { return false; }
	Entity* getSourceEntity() override { return this; }
	bool getCastsShadows() override { return false; }
	MaterialBase* getMaterial() override { return m_material; }
	IAnimator* getAnimator() override { return nullptr; }
	Matrix* getUVTransform() override { return nullptr; }
	int getTriangleCount() override { return m_segments.size(); }
	int getVertexStride() override { return 6; }
	void activateVertexBuffer(int index, IContext* context) override;
	void activateUVBuffer(int index, IContext* context) override {}
	void activateSecondaryUVBuffer(int index, IContext* context) override {}
	void activateVertexNormalBuffer(int index, IContext* context) override {}
	void activateVertexTangentBuffer(int index, IContext* context) override {}
	IndexBuffer* getIndexBuffer(IContext* context) override { return nullptr; }
	float* getVertexData() override { return nullptr; }
	float* getVertexNormalData() override { return nullptr; }
	float* getVertexTangentData() override { return nullptr; }
	unsigned short* getIndexData() override { return nullptr; }
	float* getUVData() override { return nullptr; }

protected:
	EntityNode* createEntityPartitionNode() override;
	void updateBounds() override;

protected:
	std::vector<Segment*> m_segments;

private:
	MaterialBase* m_material;
	std::vector<float> m_vertices;
	IContext* m_vertexContext;
	VertexBuffer* m_vertexBuffer;
	bool m_vertexBufferDirty;
};

AWAY_NAMESPACE_END