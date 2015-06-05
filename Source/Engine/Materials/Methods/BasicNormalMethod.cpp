#include "BasicNormalMethod.h"
#include "MethodVO.h"
#include "IContext.h"
#include "Texture2DBase.h"
#include "ShaderRegisterCache.h"
#include "ShaderRegisterData.h"
#include "ShaderChunk.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

BasicNormalMethod::BasicNormalMethod()
{
	m_normalTextureRegister = 0;
	m_texture = nullptr;
	m_useTexture = false;
}

void BasicNormalMethod::initVO(MethodVO* vo)
{
	vo->m_needsUV = m_useTexture;
}

void BasicNormalMethod::copyFrom(ShadingMethodBase* method)
{
	setNormalMap(((BasicNormalMethod*)method)->getNormalMap());
}

void BasicNormalMethod::setNormalMap(Texture2DBase* value)
{
	bool useTexture = value ? true : false;
	if (useTexture != m_useTexture)
		invalidateShaderProgram();

	m_useTexture = useTexture;
	m_texture = value;
}

void BasicNormalMethod::cleanCompilationData()
{
	m_normalTextureRegister = 0;
}

void BasicNormalMethod::activate(MethodVO* vo, IContext* context)
{
	if (vo->m_texturesIndex >= 0)
	{
		bool useMipMaps = vo->m_useMipmapping && m_texture->hasMipMaps();
		context->setSamplerStateAt(vo->m_texturesIndex, vo->m_repeatTextures ? WrapMode::REPEAT : WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(vo->m_texturesIndex, m_texture->getTextureForContext(context));
	}
}

void BasicNormalMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	m_normalTextureRegister = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(m_normalTextureRegister);

	getTex2DSampleCode(code, targetReg, m_normalTextureRegister, m_texture, m_sharedRegisters->m_uvVarying);
	code.sub(targetReg ^ Regs::xyz, targetReg, m_sharedRegisters->m_commons ^ Regs::x);
	code.nrm(targetReg ^ Regs::xyz, targetReg);
}