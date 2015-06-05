#include "LightMapMethod.h"
#include "MethodVO.h"
#include "IContext.h"
#include "Texture2DBase.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ShaderRegisterData.h"

USING_AWAY_NAMESPACE

LightMapMethod::LightMapMethod(Texture2DBase* texture)
{
	m_texture = texture;
}

void LightMapMethod::setUVTransform(float scaleU, float scaleV, float offsetU, float offsetV)
{
	m_scaleU = scaleU;
	m_scaleV = scaleV;
	m_offsetU = offsetU;
	m_offsetV = offsetV;
}

void LightMapMethod::setColorScale(float scaleR, float scaleG, float scaleB)
{
	m_scaleR = scaleR;
	m_scaleG = scaleG;
	m_scaleB = scaleB;
}

void LightMapMethod::initVO(MethodVO* vo)
{
	vo->m_needsSecondaryUV = true;
}

void LightMapMethod::initConstants(MethodVO* vo)
{
	std::vector<float>& vertexData = *(vo->m_vertexData);
	int index = vo->m_vertexConstantsIndex;
	vertexData[index] = m_scaleU;
	vertexData[index + 1] = m_scaleV;
	vertexData[index + 2] = m_offsetU;
	vertexData[index + 3] = m_offsetV;

	std::vector<float>& fragmentData = *vo->m_fragmentData;
	index = vo->m_fragmentConstantsIndex;
	fragmentData[index] = m_scaleR;
	fragmentData[index + 1] = m_scaleG;
	fragmentData[index + 2] = m_scaleB;
	fragmentData[index + 3] = 1;
}

void LightMapMethod::activate(MethodVO* vo, IContext* context)
{
	bool useMipMaps = vo->m_useMipmapping && m_texture->hasMipMaps();
	context->setSamplerStateAt(vo->m_texturesIndex, WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
	context->setTextureAt(vo->m_texturesIndex, m_texture->getTextureForContext(context));
}

void LightMapMethod::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	unsigned int lightUVConst = regCache->getFreeVertexConstant();
	vo->m_vertexConstantsIndex = REGISTER_INDEX(lightUVConst) * 4;

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	code.mul(temp ^ Regs::xy, m_sharedRegisters->m_secondaryUVInput ^ Regs::xy, lightUVConst ^ Regs::xy);
	code.add(m_sharedRegisters->m_secondaryUVVarying ^ Regs::xy, temp ^ Regs::xy, lightUVConst ^ Regs::zw);
}

void LightMapMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int colorScale = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(colorScale) * 4;
	unsigned int lightMapReg = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(lightMapReg);

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	getTex2DSampleCode(code, temp, lightMapReg, m_texture, m_sharedRegisters->m_secondaryUVVarying);
	code.mul(temp ^ Regs::xyz, temp, colorScale);
	code.mul(targetReg ^ Regs::xyz, targetReg, temp);
}