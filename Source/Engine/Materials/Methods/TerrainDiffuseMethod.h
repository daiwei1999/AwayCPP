#pragma once

#include "Common.h"
#include "BasicDiffuseMethod.h"

AWAY_NAMESPACE_BEGIN

class TerrainDiffuseMethod : public BasicDiffuseMethod
{
public:
	TerrainDiffuseMethod(int numLayers, bool useLightMap = true, bool useDistMask = true);

	bool getUseLightMap() { return m_useLightMap; }
	void setUseLightMap(bool value);
	bool getUseDistMask() { return m_useDistMask; }
	void setUseDistMask(bool value);
	Texture2DBase* getLayerMapTexture(int index) { return m_layerMaps[index]; }
	void setLayerMapTexture(int index, Texture2DBase* layerMap) { m_layerMaps[index] = layerMap; }
	Texture2DBase* getBlendMapTexture() { return m_blendMap; }
	void setBlendMapTexture(Texture2DBase* blendMap) { m_blendMap = blendMap; }
	Texture2DBase* getLightMapTexture() { return m_lightMap; }
	void setLightMapTexture(Texture2DBase* lightMap) { m_lightMap = lightMap; }

	void setBlendUVData(float scaleU, float scaleV, float offsetU, float offsetV);
	void setLayerUVData(float scaleU, float scaleV, float offsetU, float offsetV);
	void setLightUVData(float scaleU, float scaleV, float offsetU, float offsetV);
	void setLightScaleData(float scaleR, float scaleG, float scaleB);
	void setLayerTiling(int index, float tiling) { m_layerTilings[index] = tiling; }
	void setDistMaskStrength(int index, float distMaskStrength) { m_distMaskStrengths[index] = distMaskStrength; }

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void cleanCompilationData() override;

	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	void activate(MethodVO* vo, IContext* context) override;

private:
	int m_numLayers;
	bool m_useLightMap;
	bool m_useDistMask;
	Texture2DBase* m_blendMap;
	Texture2DBase* m_lightMap;
	std::vector<Texture2DBase*> m_layerMaps;
	float m_blendUVData[4];
	float m_layerUVData[4];
	float m_lightUVData[4];
	float m_lightScaleData[4];
	float m_layerTilings[4];
	float m_distMaskStrengths[4];

	unsigned int m_blendUVVaring;
	unsigned int m_layerUVVaring;
	unsigned int m_lightUVVaring;
};

AWAY_NAMESPACE_END