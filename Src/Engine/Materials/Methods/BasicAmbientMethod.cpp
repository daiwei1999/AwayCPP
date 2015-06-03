#include "BasicAmbientMethod.h"
#include "MethodVO.h"
#include "IContext.h"
#include "Texture2DBase.h"
#include "ShaderRegisterCache.h"
#include "ShaderRegisterData.h"
#include "ShaderChunk.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

BasicAmbientMethod::BasicAmbientMethod()
{
	m_useTexture = false;
	m_ambientInputRegister = 0;
	m_texture = nullptr;
	m_ambientColor = 0xffffff;
	m_ambientR = m_ambientG = m_ambientB = 0;
	m_ambient = 1;
	m_lightAmbientR = m_lightAmbientG = m_lightAmbientB = 0;
}

void BasicAmbientMethod::initVO(MethodVO* vo)
{
	vo->m_needsUV = m_useTexture;
}

void BasicAmbientMethod::initConstants(MethodVO* vo)
{
	(*vo->m_fragmentData)[vo->m_fragmentConstantsIndex + 3] = 1;
}

void BasicAmbientMethod::cleanCompilationData()
{
	m_ambientInputRegister = 0;
}

void BasicAmbientMethod::copyFrom(ShadingMethodBase* method)
{
	BasicAmbientMethod* basicAmbientMethod = (BasicAmbientMethod*)method;
	setAmbient(basicAmbientMethod->getAmbient());
	setAmbientColor(basicAmbientMethod->getAmbientColor());
}

void BasicAmbientMethod::setTexture(Texture2DBase* value)
{
	bool useTexture = value ? true : false;
	if (useTexture != m_useTexture)
		invalidateShaderProgram();

	m_useTexture = useTexture;
	m_texture = value;
}

void BasicAmbientMethod::activate(MethodVO* vo, IContext* context)
{
	if (m_useTexture)
	{
		bool useMipMaps = vo->m_useMipmapping && m_texture->hasMipMaps();
		context->setSamplerStateAt(vo->m_texturesIndex, vo->m_repeatTextures ? WrapMode::REPEAT : WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(vo->m_texturesIndex, m_texture->getTextureForContext(context));
	}
}

void BasicAmbientMethod::setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera)
{
	m_ambientR = ((m_ambientColor >> 16) & 0xff) / 255.f * m_ambient * m_lightAmbientR;
	m_ambientG = ((m_ambientColor >> 8) & 0xff) / 255.f * m_ambient * m_lightAmbientG;
	m_ambientB = (m_ambientColor & 0xff) / 255.f * m_ambient * m_lightAmbientB;

	if (!m_useTexture)
	{
		std::vector<float>& data = *vo->m_fragmentData;
		int index = vo->m_fragmentConstantsIndex;
		data[index] = m_ambientR;
		data[index + 1] = m_ambientG;
		data[index + 2] = m_ambientB;
	}
}

void BasicAmbientMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	if (m_useTexture)
	{
		m_ambientInputRegister = regCache->getFreeTextureReg();
		vo->m_texturesIndex = REGISTER_INDEX(m_ambientInputRegister);
		getTex2DSampleCode(code, targetReg, m_ambientInputRegister, m_texture, m_sharedRegisters->m_uvVarying);
		code.div(targetReg ^ Regs::xyz, targetReg, targetReg ^ Regs::w);
	}
	else
	{
		m_ambientInputRegister = regCache->getFreeFragmentConstant();
		vo->m_fragmentConstantsIndex = REGISTER_INDEX(m_ambientInputRegister) * 4;
		code.mov(targetReg, m_ambientInputRegister);
	}
}

