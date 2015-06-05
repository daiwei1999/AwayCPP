#pragma once

#include "Common.h"
#include "MaterialPassBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * SkyBoxPass provides a material pass exclusively used to render sky boxes from a cube texture.
 */
class SkyBoxPass : public MaterialPassBase
{
public:
	SkyBoxPass(CubeTextureBase* cubeMap);

	CubeTextureBase* getCubeMap() { return m_cubeMap; }
	void setCubeMap(CubeTextureBase* value) { m_cubeMap = value; }

	void getVertexCode(Shader& vertexShader) override;
	void getFragmentCode(Shader& fragmentShader) override;
	void render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection) override;
	void activate(IContext* context, Camera3D* camera) override;

private:
	CubeTextureBase* m_cubeMap;
	float m_vertexData[8];
};

AWAY_NAMESPACE_END