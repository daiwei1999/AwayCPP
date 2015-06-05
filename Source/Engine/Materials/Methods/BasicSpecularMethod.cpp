#include "BasicSpecularMethod.h"
#include "MethodVO.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "Tex.h"
#include "Texture2DBase.h"
#include "ShaderRegisterData.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

BasicSpecularMethod::BasicSpecularMethod()
{
	m_useTexture = false;
	m_gloss = 50;
	m_specular = 1;
	m_specularColor = 0xffffff;
	m_specularR = m_specularG = m_specularB = 1;
	m_isFirstLight = false;
}

void BasicSpecularMethod::setSpecular(float value)
{
	if (value != m_specular)
	{
		m_specular = value;
		updateSpecular();
	}
}

void BasicSpecularMethod::setSpecularColor(unsigned int value)
{
	if (value != m_specularColor)
	{
		if (value == 0 || m_specularColor == 0)
			invalidateShaderProgram();

		m_specularColor = value;
		updateSpecular();
	}
}

void BasicSpecularMethod::setTexture(Texture2DBase* value)
{
	bool useTexture = value ? true : false;
	if (useTexture != m_useTexture)
		invalidateShaderProgram();

	m_useTexture = useTexture;
	m_texture = value;
}

void BasicSpecularMethod::initVO(MethodVO* vo)
{
	vo->m_needsUV = m_useTexture;
	vo->m_needsNormals = vo->m_numLights > 0;
	vo->m_needsView = vo->m_numLights > 0;
}

void BasicSpecularMethod::cleanCompilationData()
{
	m_shadowRegister = 0;
	m_totalLightColorReg = 0;
	m_specularTexData = 0;
	m_specularDataRegister = 0;
}

void BasicSpecularMethod::copyFrom(ShadingMethodBase* method)
{
	BasicSpecularMethod* basicSpecularMethod = (BasicSpecularMethod*)method;
	setTexture(basicSpecularMethod->getTexture());
	setSpecular(basicSpecularMethod->getSpecular());
	setSpecularColor(basicSpecularMethod->getSpecularColor());
	setGloss(basicSpecularMethod->getGloss());
}

void BasicSpecularMethod::getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	m_isFirstLight = true;
	if (vo->m_numLights > 0)
	{
		m_specularDataRegister = regCache->getFreeFragmentConstant();
		vo->m_fragmentConstantsIndex = REGISTER_INDEX(m_specularDataRegister) * 4;

		if (m_useTexture)
		{
			m_specularTexData = regCache->getFreeFragmentVectorTemp();
			regCache->addFragmentTempUsages(m_specularTexData, 1);

			unsigned int specularTextureRegister = regCache->getFreeTextureReg();
			vo->m_texturesIndex = REGISTER_INDEX(specularTextureRegister);

			getTex2DSampleCode(code, m_specularTexData, specularTextureRegister, m_texture, m_sharedRegisters->m_uvVarying);
		}

		m_totalLightColorReg = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(m_totalLightColorReg, 1);
	}
}

void BasicSpecularMethod::getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache)
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

	// blinn-phong half vector model
	code.add(t, lightDirReg, m_sharedRegisters->m_viewDirFragment);
	code.nrm(t ^ Regs::xyz, t);
	code.dp3(t ^ Regs::w, m_sharedRegisters->m_normalFragment, t);
	code.max(t ^ Regs::w, t ^ Regs::w, m_sharedRegisters->m_commons ^ Regs::y);

	if (m_useTexture)
	{
		// apply gloss modulation from texture
		code.mul(m_specularTexData ^ Regs::w, m_specularTexData ^ Regs::y, m_specularDataRegister ^ Regs::w);
		code.pow(t ^ Regs::w, t ^ Regs::w, m_specularTexData ^ Regs::w);
	}
	else
		code.pow(t ^ Regs::w, t ^ Regs::w, m_specularDataRegister ^ Regs::w);

	// attenuate
	if (vo->m_useLightFallOff)
		code.mul(t ^ Regs::w, t ^ Regs::w, lightDirReg ^ Regs::w);

	if (m_modulateMethod)
		m_modulateMethod(code, vo, t, regCache, m_sharedRegisters);

	code.mul(t ^ Regs::xyz, lightColReg, t ^ Regs::w);

	if (!m_isFirstLight)
	{
		code.add(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, t);
		regCache->removeFragmentTempUsage(t);
	}

	m_isFirstLight = false;
}

void BasicSpecularMethod::getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache)
{
	unsigned int t;

	// write in temporary if not first light, so we can add to total diffuse colour
	if (m_isFirstLight)
	{
		t = m_totalLightColorReg;
	}
	else
	{
		t = regCache->getFreeFragmentVectorTemp();
		regCache->addFragmentTempUsages(t, 1);
	}

	unsigned int normalReg = m_sharedRegisters->m_normalFragment;
	unsigned int viewDirReg = m_sharedRegisters->m_viewDirFragment;
	code.dp3(t ^ Regs::w, normalReg, viewDirReg);
	code.add(t ^ Regs::w, t ^ Regs::w, t ^ Regs::w);
	code.mul(t, t ^ Regs::w, normalReg);
	code.sub(t, t, viewDirReg);
	code.tex(t, t, cubeMapReg | Tex::CUBE);
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

void BasicSpecularMethod::getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	if (vo->m_numLights == 0)
		return;

	if (m_shadowRegister)
		code.mul(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, m_shadowRegister ^ Regs::w);

	if (m_useTexture)
	{
		// apply strength modulation from texture
		code.mul(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, m_specularTexData ^ Regs::x);
		regCache->removeFragmentTempUsage(m_specularTexData);
	}

	// apply material's specular reflection
	code.mul(m_totalLightColorReg ^ Regs::xyz, m_totalLightColorReg, m_specularDataRegister);
	code.add(targetReg ^ Regs::xyz, targetReg, m_totalLightColorReg);
	regCache->removeFragmentTempUsage(m_totalLightColorReg);
}

void BasicSpecularMethod::activate(MethodVO* vo, IContext* context)
{
	if (vo->m_numLights == 0)
		return;

	if (m_useTexture)
	{
		context->setSamplerStateAt(vo->m_texturesIndex, WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, MipFilter::MIPLINEAR);
		context->setTextureAt(vo->m_texturesIndex, m_texture->getTextureForContext(context));
	}

	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	data[index] = m_specularR;
	data[index + 1] = m_specularG;
	data[index + 2] = m_specularB;
	data[index + 3] = m_gloss;
}

void BasicSpecularMethod::updateSpecular()
{
	m_specularR = ((m_specularColor >> 16) & 0xff) / 255.f * m_specular;
	m_specularG = ((m_specularColor >> 8) & 0xff) / 255.f * m_specular;
	m_specularB = (m_specularColor & 0xff) / 255.f * m_specular;
}