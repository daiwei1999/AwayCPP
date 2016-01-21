#include "EnvMapMethod.h"
#include "MethodVO.h"
#include "IContext.h"
#include "CubeTextureBase.h"
#include "Texture2DBase.h"
#include "ShaderRegisterCache.h"
#include "ShaderRegisterData.h"
#include "ShaderChunk.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

EnvMapMethod::EnvMapMethod(CubeTextureBase* envMap, float alpha)
{
	m_envMap = envMap;
	m_mask = nullptr;
	m_alpha = alpha;
}

void EnvMapMethod::setMask(Texture2DBase* value)
{
	if (value != m_mask)
	{
		if (!value || !m_mask)
			invalidateShaderProgram();

		m_mask = value;
	}
}

void EnvMapMethod::initVO(MethodVO* vo)
{
	vo->m_needsNormals = true;
	vo->m_needsView = true;
	vo->m_needsUV = (m_mask != nullptr);
}

void EnvMapMethod::activate(MethodVO* vo, IContext* context)
{
	(*vo->m_fragmentData)[vo->m_fragmentConstantsIndex] = m_alpha;

	bool useMipMaps = vo->m_useMipmapping && m_envMap->hasMipMaps();
	context->setSamplerStateAt(vo->m_texturesIndex, WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
	context->setTextureAt(vo->m_texturesIndex, m_envMap->getTextureForContext(context));

	if (m_mask)
	{
		useMipMaps = vo->m_useMipmapping && m_mask->hasMipMaps();
		context->setSamplerStateAt(vo->m_texturesIndex + 1, vo->m_repeatTextures ? WrapMode::REPEAT : WrapMode::CLAMP, vo->m_useSmoothTextures ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(vo->m_texturesIndex + 1, m_mask->getTextureForContext(context));
	}
}

void EnvMapMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int dataReg = regCache->getFreeFragmentConstant();
	unsigned int envMapReg = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(envMapReg);
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(dataReg) * 4;

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	regCache->addFragmentTempUsages(temp, 1);
	unsigned int temp2 = regCache->getFreeFragmentVectorTemp();

	code.dp3(temp ^ Regs::w, m_sharedRegisters->m_viewDirFragment, m_sharedRegisters->m_normalFragment); // dot(viewDir, normal)
	code.add(temp ^ Regs::w, temp ^ Regs::w, temp ^ Regs::w); // 2 * dot(viewDir, normal)
	code.mul(temp ^ Regs::xyz, m_sharedRegisters->m_normalFragment, temp ^ Regs::w); // 2 * dot(viewDir, normal) * normal
	code.sub(temp ^ Regs::xyz, temp, m_sharedRegisters->m_viewDirFragment); // 2 * dot(viewDir, normal) * normal - viewDir
	getTexCubeSampleCode(code, temp, envMapReg, m_envMap, temp);
	code.sub(temp2 ^ Regs::w, temp ^ Regs::w, Regs::c0 ^ Regs::x); // temp2.w = temp.w - 0.5
	code.kil(temp2 ^ Regs::w);	// if alpha is not 1 (mock texture) kill output (used for real time reflection mapping)
	code.sub(temp ^ Regs::xyz, temp, targetReg);

	if (m_mask)
	{
		unsigned int maskReg = regCache->getFreeTextureReg();
		getTex2DSampleCode(code, temp2, maskReg, m_mask, m_sharedRegisters->m_uvVarying);
		code.mul(temp ^ Regs::xyz, temp, temp2);
	}

	code.mul(temp ^ Regs::xyz, temp, dataReg ^ Regs::x); // (temp - targetReg) * alpha
	code.add(targetReg ^ Regs::xyz, targetReg, temp); // targetReg + (temp - targetReg) * alpha
	regCache->removeFragmentTempUsage(temp);
}