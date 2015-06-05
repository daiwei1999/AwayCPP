#include "ShadingMethodBase.h"
#include "MethodVO.h"
#include "ShaderRegisterData.h"
#include "ShaderChunk.h"
#include "Tex.h"
#include "IContext.h"
#include "TextureProxyBase.h"
#include "ShadingMethodEvent.h"

USING_AWAY_NAMESPACE

ShadingMethodBase::ShadingMethodBase()
{
	m_sharedRegisters = nullptr;
}

MethodVO* ShadingMethodBase::createMethodVO()
{
	return new MethodVO();
}

void ShadingMethodBase::initVO(MethodVO* vo)
{

}

void ShadingMethodBase::initConstants(MethodVO* vo)
{

}

void ShadingMethodBase::reset()
{
	cleanCompilationData();
}

void ShadingMethodBase::cleanCompilationData()
{

}

void ShadingMethodBase::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{

}

void ShadingMethodBase::activate(MethodVO* vo, IContext* context)
{

}

void ShadingMethodBase::setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera)
{

}

void ShadingMethodBase::deactivate(MethodVO* vo, IContext* context)
{

}

void ShadingMethodBase::copyFrom(ShadingMethodBase* method)
{

}

void ShadingMethodBase::getTex2DSampleCode(ShaderChunk& code, unsigned int targetReg, unsigned int inputReg, TextureProxyBase* texture, unsigned int uvReg)
{
	code.tex(targetReg, uvReg, inputReg | getFormatForTexture(texture));
}

void ShadingMethodBase::getTexCubeSampleCode(ShaderChunk& code, unsigned int targetReg, unsigned int inputReg, TextureProxyBase* texture, unsigned int uvReg)
{
	code.tex(targetReg, uvReg, inputReg | Tex::CUBE | getFormatForTexture(texture));
}

unsigned int ShadingMethodBase::getFormatForTexture(TextureProxyBase* texture)
{
	switch (texture->getFormat())
	{
	case TextureFormat::COMPRESSED:
		return Tex::DXT1;
	case TextureFormat::COMPRESSED_ALPHA:
		return Tex::DXT5;
	default:
		return Tex::RGBA;
	}
}

void ShadingMethodBase::invalidateShaderProgram()
{
	ShadingMethodEvent event(ShadingMethodEvent::SHADER_INVALIDATED);
	dispatchEvent(&event);
}