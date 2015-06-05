#pragma once

#include "Common.h"
#include "IRenderable.h"
#include "Matrix.h"
#include "Mesh.h"
#include "ISubGeometry.h"

AWAY_NAMESPACE_BEGIN

/**
 * SubMesh wraps a SubGeometry as a scene graph instantiation. A SubMesh is owned by a Mesh object.
 */
class SubMesh : public IRenderable
{
public:
	SubMesh(ISubGeometry* subGeometry, Mesh* parentMesh, MaterialBase* material = nullptr);

	ISubGeometry* getSubGeometry() { return m_subGeometry; }
	void setSubGeometry(ISubGeometry* value) { m_subGeometry = value; }

	MaterialBase* getMaterial() override { return m_material ? m_material : m_parentMesh->getMaterial(); }
	void setMaterial(MaterialBase* value);

	float getOffsetU() { return m_offsetU; }
	void setOffsetU(float value);
	float getOffsetV() { return m_offsetV; }
	void setOffsetV(float value);
	float getScaleU() { return m_scaleU; }
	void setScaleU(float value);
	float getScaleV() { return m_scaleV; }
	void setScaleV(float value);
	float getUVRotation() { return m_uvRotation; }
	void setUVRotation(float value);

	Matrix3D& getRenderSceneTransform(Camera3D* camera) override { return m_parentMesh->getSceneTransform(); }
	Matrix3D& getInverseSceneTransform() override { return m_parentMesh->getInverseSceneTransform();	}
	IAnimator* getAnimator() override { return m_parentMesh->getAnimator(); }
	bool getMouseEnabled() override { return m_parentMesh->getMouseEnabled() || m_parentMesh->m_ancestorsAllowMouseEnabled; }
	Entity* getSourceEntity() override { return m_parentMesh; }
	bool getCastsShadows() override { return m_parentMesh->getCastsShadows(); }
	Mesh* getParentMesh() { return m_parentMesh; }
	Matrix* getUVTransform() override;
	int getTriangleCount() override { return m_subGeometry->getTriangleCount(); }
	int getVertexStride() override { return m_subGeometry->getVertexStride(); }
	void activateVertexBuffer(int index, IContext* context) override { m_subGeometry->activateVertexBuffer(index, context); }
	void activateUVBuffer(int index, IContext* context) override { m_subGeometry->activateUVBuffer(index, context); }
	void activateSecondaryUVBuffer(int index, IContext* context) override { m_subGeometry->activateSecondaryUVBuffer(index, context); }
	void activateVertexNormalBuffer(int index, IContext* context) override { m_subGeometry->activateVertexNormalBuffer(index, context); }
	void activateVertexTangentBuffer(int index, IContext* context) override { m_subGeometry->activateVertexTangentBuffer(index, context); }
	IndexBuffer* getIndexBuffer(IContext* context) override { return m_subGeometry->getIndexBuffer(context); }
	float* getVertexData() override { return m_subGeometry->getVertexData(); }
	float* getVertexNormalData() override { return m_subGeometry->getVertexNormalData(); }
	float* getVertexTangentData() override { return m_subGeometry->getVertexTangentData(); }
	unsigned short* getIndexData() override { return m_subGeometry->getIndexData(); }
	float* getUVData() override { return m_subGeometry->getUVData(); }

private:
	void updateUVTransform();

public:
	AnimationSubGeometry* m_animationSubGeometry;
	AnimationSubGeometry* m_animatorSubGeometry;

private:
	Mesh* m_parentMesh;
	ISubGeometry* m_subGeometry;
	MaterialBase* m_material;
	float m_uvRotation;
	float m_scaleU;
	float m_scaleV;
	float m_offsetU;
	float m_offsetV;
	Matrix m_uvTransform;
	bool m_uvTransformDirty;
};

AWAY_NAMESPACE_END