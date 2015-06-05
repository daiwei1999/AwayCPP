#pragma once

#include "Common.h"
#include "LightBase.h"

AWAY_NAMESPACE_BEGIN

class LightProbe : public LightBase
{
public:
	LightProbe(CubeTextureBase* diffuseMap, CubeTextureBase* specularMap = nullptr);

	CubeTextureBase* getDiffuseMap() { return m_diffuseMap; }
	void setDiffuseMap(CubeTextureBase* value) { m_diffuseMap = value; }
	CubeTextureBase* getSpecularMap() { return m_specularMap; }
	void setSpecularMap(CubeTextureBase* value) { m_specularMap = value; }

	void getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result) override;

protected:
	void updateBounds() override;
	EntityNode* createEntityPartitionNode() override;

private:
	CubeTextureBase* m_diffuseMap;
	CubeTextureBase* m_specularMap;
};

AWAY_NAMESPACE_END