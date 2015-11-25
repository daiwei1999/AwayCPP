#include "CelSpecularMethod.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "MethodVO.h"
#include "Regs.h"

USING_AWAY_NAMESPACE
using namespace std::placeholders;

CelSpecularMethod::CelSpecularMethod(float specularCutOff, BasicSpecularMethod* baseSpecularMethod) : CompositeSpecularMethod(std::bind(&CelSpecularMethod::clampSpecular, this, _1, _2, _3, _4, _5), baseSpecularMethod)
{
	m_smoothness = 0.1f;
	m_specularCutOff = specularCutOff;
}

void CelSpecularMethod::activate(MethodVO* vo, IContext* context)
{
	CompositeSpecularMethod::activate(vo, context);

	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_secondaryFragmentConstantsIndex;
	data[index] = m_smoothness;
	data[index + 1] = m_specularCutOff;
}

void CelSpecularMethod::getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	m_dataReg = regCache->getFreeFragmentConstant();
	vo->m_secondaryFragmentConstantsIndex = REGISTER_INDEX(m_dataReg) * 4;
	CompositeSpecularMethod::getFragmentPreLightingCode(code, vo, regCache);
}

void CelSpecularMethod::clampSpecular(ShaderChunk& code, MethodVO* vo, unsigned int t, ShaderRegisterCache* regCache, ShaderRegisterData* sharedRegisters)
{
	unsigned int ty = t ^ Regs::y;
	unsigned int tw = t ^ Regs::w;
	code.sub(ty, tw, m_dataReg ^ Regs::y);
	code.div(ty, ty, m_dataReg ^ Regs::x);
	code.sat(ty, ty);
	code.sge(tw, tw, m_dataReg ^ Regs::y);
	code.mul(tw, tw, ty);
}