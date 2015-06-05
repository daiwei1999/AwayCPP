#pragma once

#include "Common.h"
#include "MaterialBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * SkyBoxMaterial is a material exclusively used to render skyboxes
 */
class SkyBoxMaterial : public MaterialBase
{
public:
	SkyBoxMaterial(CubeTextureBase* cubeMap);

	CubeTextureBase* getCubeMap() { return m_cubeMap; }
	void setCubeMap(CubeTextureBase* value);

private:
	CubeTextureBase* m_cubeMap;
	SkyBoxPass* m_skyboxPass;
};

AWAY_NAMESPACE_END