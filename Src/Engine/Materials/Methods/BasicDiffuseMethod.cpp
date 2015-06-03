#include "BasicDiffuseMethod.h"
#include "MethodVO.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "Tex.h"
#include "ShaderRegisterData.h"
#include "IContext.h"
#include "Texture2DBase.h"

USING_AWAY_NAMESPACE

BasicDiffuseMethod::BasicDiffuseMethod()
{
	m_useTexture = false;
	m_isFirstLight = false;
	m_totalLightColorReg = m_diffuseInputRegister = m_shadowRegister = 0;
	m_alphaThreshold = 0;
	m_useAmbientTexture = false;
	m_texture = nullptr;
	m_diffuseColor = 0xffffff;
	m_diffuseR = m_diffuseG = m_diffuseB = m_diffuseA = 1;
}

void BasicDiffuseMethod::setUseAmbientTexture(bool value)
{
	if (m_useAmbientTexture != value)
	{
		m_useAmbientTexture = value;
		invalidateShaderProgram();
	}
}

void BasicDiffuseMethod::setDiffuseColor(unsigned int value)
{
	m_diffuseColor = value;
	m_diffuseR = ((m_diffuseColor >> 16) & 0xff) / 255.f;
	m_diffuseG = ((m_diffuseColor >> 8) & 0xff) / 255.f;
	m_diffuseB = (m_diffuseColor & 0xff) / 255.f;
}

void BasicDiffuseMethod::setTexture(Texture2DBase* value)
{
	bool useTexture = value ? true : false;
	if (useTexture != m_useTexture)
		invalidateShaderProgram();

	m_useTexture = useTexture;
	m_texture = value;
}

void BasicDiffuseMethod::setAlphaThreshold(float value)
{
	assert(value >= 0 && value <= 1);
	if (value != m_alphaThreshold)
	{
		if (value == 0 || m_alphaThreshold == 0)
			invalidateShaderProgram();

		m_alphaThreshold = value;
	}
}

void BasicDiffuseMethod::initVO(MethodVO* vo)
{
	vo->m_needsUV = m_useTexture;
	vo->m_needsNormals = vo->m_numLights > 0;
}

void BasicDiffuseMethod::cleanCompilationData()
{
	m_shadowRegister = 0;
	m_totalLightColorReg = 0;
	m_diffuseInputRegister = 0;
}

void BasicDiffuseMethod::copyFrom(ShadingMethodBase* method)
{
	BasicDiffuseMethod* basicDiffuseMethod = (BasicDiffuseMethod*)method;
	setAlphaThreshold(basicDiffuseMethod->getAlphaThreshold());
	setTexture(basicDiffuseMethod->getTexture());
	setUseAmbientTexture(basicDiffuseMethod->getUseAmbientTexture());
	setDiffuseAlpha(basicDiffuseMethod->getDiffuseAlpha());
	setDiffuseColor(basicDiffuseMethod->getDiffuseColor());
}

void BasicDiffuseMethod::getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	m_isFirstLight = true;
	if (vo->m_numLights > 0)
	{
		m_totalLightColorReg = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(m_totalLightColorReg, 1);
	}
}

void BasicDiffuseMethod::getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache)
{
	unsigned int t;
	if (m_isFirstLight)
	{
		t = m_totalLightColorReg;
	}
	else
	{
		t = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(t, 1);
	}

	code.dp3(t ^ Regs::x, lightDirReg, m_sharedRegisters->m_normalFragment);
	code.max(t ^ Regs::w, t ^ Regs::x, m_sharedRegisters->m_commons ^ Regs::y);

	if (vo->m_useLightFallOff)
		code.mul(t ^ Regs::w, t ^ Regs::w, lightDirReg ^ Regs::w);

	if (m_modulateMethod)
		m_modulateMethod(code, vo, t, regCache, m_sharedRegisters);

	code.mul(t, t ^ Regs::w, lightColReg);
	if (!m_isFirstLight)
	{
		code.add(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, t);
		regCache->removeFragmentTempUsage(t);
	}
	m_isFirstLight = false;
}

void BasicDiffuseMethod::getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache)
{
	unsigned int t;
	if (m_isFirstLight)
	{
		t = m_totalLightColorReg;
	}
	else
	{
		t = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(t, 1);
	}

	code.tex(t, m_sharedRegisters->m_normalFragment, cubeMapReg | Tex::CUBE);
	code.mul(t ^ Regs::xyz, t, weightRegister);

	if (m_modulateMethod)
		m_modulateMethod(code, vo, t, regCache, m_sharedRegisters);

	if (!m_isFirstLight)
	{
		code.add(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, t);
		regCache->removeFragmentTempUsage(t);
	}
	m_isFirstLight = false;
}

void BasicDiffuseMethod::getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int albedo;
	if (vo->m_numLights > 0)
	{
		if (m_shadowRegister)
			applyShadow(code, vo, regCache);

		albedo = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(albedo, 1);
	}
	else
	{
		albedo = targetReg;
	}

	if (m_useTexture)
	{
		m_diffuseInputRegister = regCache->getFreeTextureReg();
		vo->m_texturesIndex = REGISTER_INDEX(m_diffuseInputRegister);
		getTex2DSampleCode(code, albedo, m_diffuseInputRegister, m_texture, m_sharedRegisters->m_uvVarying);
		if (m_alphaThreshold > 0)
		{
			unsigned int cutOffReg = regCache->getFreeFragmentConstant();
			vo->m_fragmentConstantsIndex = REGISTER_INDEX(cutOffReg) * 4;
			code.sub(albedo ^ Regs::w, albedo ^ Regs::w, cutOffReg ^ Regs::x);
			code.kil(albedo ^ Regs::w);
			code.add(albedo ^ Regs::w, albedo ^ Regs::w, cutOffReg ^ Regs::x);
		}
	}
	else
	{
		m_diffuseInputRegister = regCache->getFreeFragmentConstant();
		vo->m_fragmentConstantsIndex = REGISTER_INDEX(m_diffuseInputRegister) * 4;
		code.mov(albedo, m_diffuseInputRegister);
	}

	if (vo->m_numLights == 0)
	{
		if (m_shadowRegister)
		{
			code.mul(m_shadowRegister ^ Regs::w, m_shadowRegister ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::x);
			code.add(m_shadowRegister ^ Regs::w, m_shadowRegister ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::x);
			code.mul(albedo ^ Regs::xyz, albedo, m_shadowRegister ^ Regs::w);
		}
		return;
	}

	code.sat(m_totalLightColorReg, m_totalLightColorReg);

	if (m_useAmbientTexture)
	{
		code.mul(albedo ^ Regs::xyz, albedo, m_totalLightColorReg);
		code.mul(m_totalLightColorReg ^ Regs::xyz, targetReg, m_totalLightColorReg);
		code.sub(targetReg ^ Regs::xyz, targetReg, m_totalLightColorReg);
		code.add(targetReg ^ Regs::xyz, albedo, targetReg);
	}
	else
	{
		code.add(targetReg ^ Regs::xyz, m_totalLightColorReg, targetReg);
		if (m_useTexture)
		{
			code.mul(targetReg ^ Regs::xyz, albedo, targetReg);
			code.mov(targetReg ^ Regs::w, albedo ^ Regs::w);
		}
		else
		{
			code.mul(targetReg ^ Regs::xyz, m_diffuseInputRegister, targetReg);
			code.mov(targetReg ^ Regs::w, m_diffuseInputRegister ^ Regs::w);
		}
	}

	regCache->removeFragmentTempUsage(m_totalLightColorReg);
	regCache->removeFragmentTempUsage(albedo);
}

void BasicDiffuseMethod::activate(MethodVO* vo, IContext* context)
{
	if (m_useTexture)
	{
		bool useMipMaps = vo->m_useMipmapping && m_texture->hasMipMaps();
		context->setSamplerStateAt(vo->m_texturesIndex, vo->m_repeatTextures ? WrapMode::REPEAT : WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(vo->m_texturesIndex, m_texture->getTextureForContext(context));
		if (m_alphaThreshold > 0)
		{
			(*vo->m_fragmentData)[vo->m_fragmentConstantsIndex] = m_alphaThreshold;
		}
	}
	else
	{
		std::vector<float>& data = *vo->m_fragmentData;
		int index = vo->m_fragmentConstantsIndex;
		data[index] = m_diffuseR;
		data[index + 1] = m_diffuseG;
		data[index + 2] = m_diffuseB;
		data[index + 3] = m_diffuseA;
	}
}

void BasicDiffuseMethod::applyShadow(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	code.mul(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, m_shadowRegister ^ Regs::w);
}