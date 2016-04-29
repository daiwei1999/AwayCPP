#include "DistortionMethod.h"
#include "MethodVO.h"
#include "IContext.h"
#include "Texture.h"
#include "Texture2DBase.h"
#include "ShaderRegisterCache.h"
#include "ShaderRegisterData.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "Tex.h"

USING_AWAY_NAMESPACE

DistortionMethod::DistortionMethod(Texture2DBase* normalMap, Texture* sceneTexture, float distortUFactor, float distortVFactor, float distortFactor, float alphaFactor)
{
	m_normalMap = normalMap;
	m_sceneTexture = sceneTexture;
	m_distortUFactor = distortUFactor;
	m_distortVFactor = distortVFactor;
	m_distortFactor = distortFactor;
	m_alphaFactor = alphaFactor;
}

void DistortionMethod::initVO(MethodVO* vo)
{
	vo->m_needsProjection = true;
}

void DistortionMethod::initConstants(MethodVO* vo)
{
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	data[index] = 0.5f;
	data[index + 1] = -0.5f;
	data[index + 2] = m_distortFactor;
	data[index + 3] = m_alphaFactor;
}

void DistortionMethod::activate(MethodVO* vo, IContext* context)
{
	float time = 0;
	std::vector<float>& data = *vo->m_vertexData;
	int index = vo->m_vertexConstantsIndex;
	data[index] = m_distortUFactor * time;
	data[index + 1] = m_distortVFactor * time;

	index = vo->m_texturesIndex;
	context->setSamplerStateAt(index, WrapMode::REPEAT, TextureFilter::LINEAR, MipFilter::MIPLINEAR);
	context->setTextureAt(index++, m_normalMap->getTextureForContext(context));

	context->setSamplerStateAt(index, WrapMode::REPEAT, TextureFilter::LINEAR, MipFilter::MIPNONE);
	context->setTextureAt(index, m_sceneTexture);
}

void DistortionMethod::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	unsigned int distortUVFactorReg = regCache->getFreeVertexConstant();
	vo->m_vertexConstantsIndex = REGISTER_INDEX(distortUVFactorReg) * 4;

	m_distortUVVaring = regCache->getFreeVarying();
	code.add(m_distortUVVaring, m_sharedRegisters->m_uvInput ^ Regs::xy, distortUVFactorReg ^ Regs::xy);
}

void DistortionMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int normalTexReg = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(normalTexReg);
	unsigned int sceneTexReg = regCache->getFreeTextureReg();
	unsigned int dataReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(dataReg) * 4;

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	regCache->addFragmentTempUsages(temp, 1);
	unsigned int temp2 = regCache->getFreeFragmentVectorTemp();
	getTex2DSampleCode(code, temp, normalTexReg, m_normalMap, m_distortUVVaring); // sampler from normalMap
	code.sub(temp ^ Regs::xy, temp ^ Regs::xy, dataReg ^ Regs::x); // normal - 0.5
	code.sat(temp ^ Regs::xy, temp ^ Regs::xy); // distortVector
	code.mul(temp ^ Regs::xy, temp ^ Regs::xy, dataReg ^ Regs::z); // distortVector * distortFactor
	code.mul(temp ^ Regs::xy, temp ^ Regs::xy, temp ^ Regs::w); // distortVector * distortFactor * normal.a

	code.div(temp2, m_sharedRegisters->m_projectivePositionVarying, m_sharedRegisters->m_projectivePositionVarying ^ Regs::w);
	code.mul(temp2 ^ Regs::xy, temp2 ^ Regs::xy, dataReg ^ Regs::xy);
	code.add(temp2 ^ Regs::xy, temp2 ^ Regs::xy, dataReg ^ Regs::x);
	code.add(temp2 ^ Regs::xy, temp2 ^ Regs::xy, temp ^ Regs::xy);

	// alpha blend
	code.tex(temp, temp2, sceneTexReg | Tex::RGBA); // sampler from sceneTexture
	code.mul(temp2 ^ Regs::xyz, targetReg ^ Regs::xyz, targetReg ^ Regs::w); // targetReg.rgb * targetReg.a
	code.mul(temp2 ^ Regs::xyz, temp2 ^ Regs::xyz, dataReg ^ Regs::w); // targetReg.rgb * targetReg.a * alphaFactor
	code.add(targetReg ^ Regs::xyz, temp ^ Regs::xyz, temp2 ^ Regs::xyz); // sceneColor.rgb + targetReg.rgb * targetReg.a * alphaFactor
}