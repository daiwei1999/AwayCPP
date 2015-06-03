#pragma once

#include "Common.h"
#include "Entity.h"
#include "IRenderable.h"

AWAY_NAMESPACE_BEGIN

/**
 * A SkyBox class is used to render a sky in the scene. It's always considered static and 'at infinity', and as
 * such it's always centered at the camera's position and sized to exactly fit within the camera's frustum, ensuring
 * the sky box is always as large as possible without being clipped.
 */
class SkyBox : public Entity, public IRenderable
{
public:
	SkyBox(CubeTextureBase* cubeMap);

	Matrix3D& getRenderSceneTransform(Camera3D* camera) override { return m_sceneTransform; }
	Matrix3D& getInverseSceneTransform() override { return Entity::getInverseSceneTransform(); }
	bool getMouseEnabled() override { return false; }
	Entity* getSourceEntity() override { return this; }
	bool getCastsShadows() override { return false; }
	MaterialBase* getMaterial() override;
	IAnimator* getAnimator() override { return nullptr; }
	Matrix* getUVTransform() override { return nullptr; }
	int getTriangleCount() override;
	int getVertexStride() override { return 6; }
	void activateVertexBuffer(int index, IContext* context) override;
	void activateUVBuffer(int index, IContext* context) override {}
	void activateSecondaryUVBuffer(int index, IContext* context) override {}
	void activateVertexNormalBuffer(int index, IContext* context) override {}
	void activateVertexTangentBuffer(int index, IContext* context) override {}
	IndexBuffer* getIndexBuffer(IContext* context) override;
	float* getVertexData() override;
	float* getVertexNormalData() override;
	float* getVertexTangentData() override;
	unsigned short* getIndexData() override;
	float* getUVData() override;

protected:
	EntityNode* createEntityPartitionNode() override;
	void updateBounds() override;

private:
	// todo: remove SubGeometry, use a simple single buffer with offsets
	SubGeometry* m_geometry;
	SkyBoxMaterial* m_material;
	std::vector<float> m_vertices;
	std::vector<unsigned short> m_indices;
};

AWAY_NAMESPACE_END