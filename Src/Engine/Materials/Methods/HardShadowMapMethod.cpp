#include "HardShadowMapMethod.h"
#include "MethodVO.h"
#include "ShaderRegisterCache.h"
#include "ShaderRegisterData.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "Tex.h"

USING_AWAY_NAMESPACE

HardShadowMapMethod::HardShadowMapMethod(LightBase* castingLight) : SimpleShadowMapMethodBase(castingLight)
{

}

void HardShadowMapMethod::initVO(MethodVO* vo)
{
	SimpleShadowMapMethodBase::initVO(vo);
	vo->m_needsGlobalFragmentPos = m_usePoint;
}

void HardShadowMapMethod::getCascadeFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int decodeRegister, unsigned int depthTexture, unsigned int depthProjection, unsigned int targetRegister)
{
	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	code.tex(temp, depthProjection, depthTexture);
	code.dp4(temp ^ Regs::z, temp, decodeRegister);
	code.slt(targetRegister ^ Regs::w, depthProjection ^ Regs::z, temp ^ Regs::z);
}

void HardShadowMapMethod::activateForCascade(MethodVO* vo, IContext* context)
{

}

void HardShadowMapMethod::getPlanarFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int depthMapReg = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(depthMapReg);

	unsigned int decReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(decReg) * 4;
	regCache->getFreeFragmentConstant();
	
	unsigned int depthCol = regCache->getFreeFragmentVectorTemp();
	code.tex(depthCol, m_depthMapCoordReg, depthMapReg);
	code.dp4(depthCol ^ Regs::z, depthCol, decReg);
	code.slt(targetReg ^ Regs::w, m_depthMapCoordReg ^ Regs::z, depthCol ^ Regs::z); // 0 if in shadow
}

void HardShadowMapMethod::getPointFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int depthMapReg = regCache->getFreeTextureReg();
	vo->m_texturesIndex = REGISTER_INDEX(depthMapReg);

	unsigned int decReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(decReg) * 4;
	unsigned int epsReg = regCache->getFreeFragmentConstant();
	unsigned int posReg = regCache->getFreeFragmentConstant();

	unsigned int depthCol = regCache->getFreeFragmentVectorTemp();
	regCache->addFragmentTempUsages(depthCol, 1);
	unsigned int lightDir = regCache->getFreeFragmentVectorTemp();
	regCache->removeFragmentTempUsage(depthCol);

	code.sub(lightDir, m_sharedRegisters->m_globalPositionVarying, posReg);
	code.dp3(lightDir ^ Regs::w, lightDir, lightDir);
	code.mul(lightDir ^ Regs::w, lightDir ^ Regs::w, posReg ^ Regs::w);
	code.nrm(lightDir ^ Regs::xyz, lightDir);

	code.tex(depthCol, lightDir, depthMapReg | Tex::CUBE);
	code.dp4(depthCol ^ Regs::z, depthCol, decReg);
	code.add(targetReg ^ Regs::w, lightDir ^ Regs::w, epsReg ^ Regs::x);
	code.slt(targetReg ^ Regs::w, targetReg ^ Regs::w, depthCol ^ Regs::z);
}