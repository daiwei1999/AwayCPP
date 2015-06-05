#include "TerrainDiffuseMethod.h"
#include "MethodVO.h"
#include "ShaderRegisterCache.h"
#include "ShaderRegisterData.h"
#include "Texture2DBase.h"
#include "IContext.h"
#include "ShaderChunk.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

TerrainDiffuseMethod::TerrainDiffuseMethod(int numLayers, bool useLightMap, bool useDistMask)
{
	m_numLayers = numLayers;
	m_useLightMap = useLightMap;
	m_useDistMask = useDistMask;
	m_blendMap = nullptr;
	m_lightMap = nullptr;
	m_layerMaps.resize(numLayers);
	m_lightScaleData[3] = 1;

	m_blendUVVaring = 0;
	m_layerUVVaring = 0;
	m_lightUVVaring = 0;
}

void TerrainDiffuseMethod::setUseLightMap(bool value)
{
	if (value != m_useLightMap)
	{
		m_useLightMap = value;
		invalidateShaderProgram();
	}
}

void TerrainDiffuseMethod::setUseDistMask(bool value)
{
	if (value != m_useDistMask)
	{
		m_useDistMask = value;
		invalidateShaderProgram();
	}
}

void TerrainDiffuseMethod::setBlendUVData(float scaleU, float scaleV, float offsetU, float offsetV)
{
	m_blendUVData[0] = scaleU;
	m_blendUVData[1] = scaleV;
	m_blendUVData[2] = offsetU;
	m_blendUVData[3] = offsetV;
}

void TerrainDiffuseMethod::setLayerUVData(float scaleU, float scaleV, float offsetU, float offsetV)
{
	m_layerUVData[0] = scaleU;
	m_layerUVData[1] = scaleV;
	m_layerUVData[2] = offsetU;
	m_layerUVData[3] = offsetV;
}

void TerrainDiffuseMethod::setLightUVData(float scaleU, float scaleV, float offsetU, float offsetV)
{
	m_lightUVData[0] = scaleU;
	m_lightUVData[1] = scaleV;
	m_lightUVData[2] = offsetU;
	m_lightUVData[3] = offsetV;
}

void TerrainDiffuseMethod::setLightScaleData(float scaleR, float scaleG, float scaleB)
{
	m_lightScaleData[0] = scaleR;
	m_lightScaleData[1] = scaleG;
	m_lightScaleData[2] = scaleB;
}

void TerrainDiffuseMethod::initVO(MethodVO* vo)
{
	vo->m_needsUV = true;
	vo->m_needsNormals = vo->m_numLights > 0;
}

void TerrainDiffuseMethod::initConstants(MethodVO* vo)
{
	std::vector<float>& vertexData = *(vo->m_vertexData);
	int offset, index = vo->m_vertexConstantsIndex;
	for (int i = 0; i < 4; i++)
	{
		offset = index + i;
		vertexData[offset] = m_blendUVData[i];
		vertexData[offset + 4] = m_layerUVData[i];
		if (m_useLightMap)
			vertexData[offset + 8] = m_lightUVData[i];
	}

	std::vector<float>& fragmentData = *(vo->m_fragmentData);
	index = vo->m_fragmentConstantsIndex;
	for (int i = 0; i < 4; i++)
	{
		offset = index + i;
		fragmentData[offset] = m_layerTilings[i];
		fragmentData[offset + 4] = m_distMaskStrengths[i];
		if (m_useLightMap)
			fragmentData[offset + 8] = m_lightScaleData[i];
	}
}

void TerrainDiffuseMethod::cleanCompilationData()
{
	BasicDiffuseMethod::cleanCompilationData();
	m_blendUVVaring = 0;
	m_layerUVVaring = 0;
	m_lightUVVaring = 0;
}

void TerrainDiffuseMethod::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	unsigned int blendUVConst = regCache->getFreeVertexConstant();
	unsigned int layerUVConst = regCache->getFreeVertexConstant();
	vo->m_vertexConstantsIndex = REGISTER_INDEX(blendUVConst) * 4;

	m_blendUVVaring = regCache->getFreeVarying();
	m_layerUVVaring = regCache->getFreeVarying();

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	code.mul(temp ^ Regs::xy, m_sharedRegisters->m_uvInput ^ Regs::xy, blendUVConst ^ Regs::xy);
	code.add(m_blendUVVaring, temp ^ Regs::xy, blendUVConst ^ Regs::zw);
	code.mul(temp ^ Regs::xy, m_sharedRegisters->m_uvInput ^ Regs::xy, layerUVConst ^ Regs::xy);
	code.add(m_layerUVVaring, temp ^ Regs::xy, layerUVConst ^ Regs::zw);

	if (m_useLightMap)
	{
		m_lightUVVaring = regCache->getFreeVarying();
		unsigned int lightUVConst = regCache->getFreeVertexConstant();
		code.mul(temp ^ Regs::xy, m_sharedRegisters->m_uvInput ^ Regs::xy, lightUVConst ^ Regs::xy);
		code.add(m_lightUVVaring, temp ^ Regs::xy, lightUVConst ^ Regs::zw);
	}
}

void TerrainDiffuseMethod::getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int albedo;
	if (vo->m_numLights > 0)
	{
		if (m_shadowRegister)
			code.mul(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, m_shadowRegister ^ Regs::w);

		code.add(targetReg ^ Regs::xyz, m_totalLightColorReg, targetReg);
		code.sat(targetReg ^ Regs::xyz, targetReg);
		regCache->removeFragmentTempUsage(m_totalLightColorReg);

		albedo = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(albedo, 1);
	}
	else
		albedo = targetReg;

	unsigned int blendReg = regCache->getFreeFragmentVectorTemp();
	regCache->addFragmentTempUsages(blendReg, 1);
	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	unsigned int layerTileReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(layerTileReg) * 4;
	unsigned int distMaskReg = regCache->getFreeFragmentConstant();
	unsigned int blendTexReg = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(blendTexReg);
	std::vector<unsigned int> layerTexRegs(m_numLayers);
	for (int i = 0; i < m_numLayers; i++)
		layerTexRegs[i] = regCache->getFreeTextureReg();

	code.mov(albedo, m_sharedRegisters->m_commons ^ Regs::y);
	getTex2DSampleCode(code, blendReg, blendTexReg, m_blendMap, m_blendUVVaring);

	if (m_blendMap->getFormat() == TextureFormat::COMPRESSED)
	{
		code.add(blendReg ^ Regs::w, blendReg ^ Regs::x, blendReg ^ Regs::y);
		code.add(blendReg ^ Regs::w, blendReg ^ Regs::w, blendReg ^ Regs::z);
		code.sub(blendReg ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::w, blendReg ^ Regs::w);
	}

	// Layer Texture Blend
	unsigned int components[4] = { Regs::x, Regs::y, Regs::z, Regs::w };
	for (int i = 0; i < m_numLayers; i++)
	{
		unsigned int component = components[i];
		code.mul(temp, m_layerUVVaring ^ Regs::xy, layerTileReg ^ component);
		getTex2DSampleCode(code, temp, layerTexRegs[i], m_layerMaps[i], temp);

		if (m_useDistMask)
		{
			code.mul(temp ^ Regs::w, temp ^ Regs::w, distMaskReg ^ component);
			code.add(temp ^ Regs::w, temp ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::w);
			code.mul(temp ^ Regs::w, temp ^ Regs::w, blendReg ^ component);
			code.mul(temp ^ Regs::xyz, temp, temp ^ Regs::w);
		}
		else
		{
			code.mul(temp ^ Regs::xyz, temp, blendReg ^ component);
		}
		
		code.add(albedo, albedo, temp);
	}

	if (m_useDistMask)
	{
		code.sub(temp ^ Regs::x, m_sharedRegisters->m_commons ^ Regs::w, albedo ^ Regs::w);
		code.max(temp ^ Regs::x, temp ^ Regs::x, m_sharedRegisters->m_commons ^ Regs::y);
		code.add(albedo ^ Regs::w, albedo ^ Regs::w, temp ^ Regs::x);
		code.div(albedo, albedo, albedo ^ Regs::w);
	}

	// Terrain Light Map
	if (m_useLightMap)
	{
		unsigned int lightScaleReg = regCache->getFreeFragmentConstant();
		unsigned int lightTexReg = regCache->getFreeTextureReg();
		getTex2DSampleCode(code, temp, lightTexReg, m_lightMap, m_lightUVVaring);
		code.mul(albedo ^ Regs::xyz, albedo, temp);
		code.mul(albedo ^ Regs::xyz, albedo, lightScaleReg);
	}
	regCache->removeFragmentTempUsage(blendReg);

	if (vo->m_numLights > 0)
	{
		code.mul(targetReg ^ Regs::xyz, albedo, targetReg);
		code.mov(targetReg ^ Regs::w, albedo ^ Regs::w);
		regCache->removeFragmentTempUsage(albedo);
	}
	else if (m_shadowRegister)
	{
		code.mul(m_shadowRegister ^ Regs::w, m_shadowRegister ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::x);
		code.add(m_shadowRegister ^ Regs::w, m_shadowRegister ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::x);
		code.mul(albedo ^ Regs::xyz, albedo, m_shadowRegister ^ Regs::w);
	}
}

void TerrainDiffuseMethod::activate(MethodVO* vo, IContext* context)
{
	int index = vo->m_texturesIndex;
	TextureFilter filter = vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST;
	bool useMipMaps = vo->m_useMipmapping && m_blendMap->hasMipMaps();
	context->setSamplerStateAt(index, WrapMode::CLAMP, filter, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
	context->setTextureAt(index++, m_blendMap->getTextureForContext(context));

	Texture2DBase* layerMap;
	for (int i = 0; i < m_numLayers; i++)
	{
		layerMap = m_layerMaps[i];
		useMipMaps = vo->m_useMipmapping && layerMap->hasMipMaps();
		context->setSamplerStateAt(index, WrapMode::REPEAT, filter, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(index++, layerMap->getTextureForContext(context));
	}

	if (m_useLightMap)
	{
		useMipMaps = vo->m_useMipmapping && m_lightMap->hasMipMaps();
		context->setSamplerStateAt(index, WrapMode::CLAMP, filter, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(index, m_lightMap->getTextureForContext(context));
	}
}