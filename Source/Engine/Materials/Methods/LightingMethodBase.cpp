#include "LightingMethodBase.h"

USING_AWAY_NAMESPACE

LightingMethodBase::LightingMethodBase()
{
	m_modulateMethod = nullptr;
}

void LightingMethodBase::getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{

}

void LightingMethodBase::getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache)
{

}

void LightingMethodBase::getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache)
{

}

void LightingMethodBase::getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{

}