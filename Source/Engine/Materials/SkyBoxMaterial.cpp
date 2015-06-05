#include "SkyBoxMaterial.h"
#include "SkyBoxPass.h"

USING_AWAY_NAMESPACE

SkyBoxMaterial::SkyBoxMaterial(CubeTextureBase* cubeMap)
{
	m_cubeMap = cubeMap;
	addPass(m_skyboxPass = new SkyBoxPass(m_cubeMap));
}

void SkyBoxMaterial::setCubeMap(CubeTextureBase* value)
{
	m_cubeMap = value;
	m_skyboxPass->setCubeMap(m_cubeMap);
}