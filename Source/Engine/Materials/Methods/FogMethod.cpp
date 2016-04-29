#include "FogMethod.h"
#include "MethodVO.h"
#include "Regs.h"
#include "ShaderChunk.h"
#include "ShaderRegisterData.h"
#include "ShaderRegisterCache.h"

USING_AWAY_NAMESPACE

FogMethod::FogMethod(float minDistance, float maxDistance, unsigned int color)
{
	m_minDistance = minDistance;
	m_maxDistance = maxDistance;
	setColor(color);
}

void FogMethod::setColor(unsigned int value)
{
	m_color = value;
	m_colorR = ((value >> 16) & 0xff) / 255.f;
	m_colorG = ((value >> 8) & 0xff) / 255.f;
	m_colorB = (value & 0xff) / 255.f;
}

void FogMethod::initVO(MethodVO* vo)
{
	vo->m_needsProjection = true;
}

void FogMethod::initConstants(MethodVO* vo)
{
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	data[index + 3] = 1;
	data[index + 6] = 0;
	data[index + 7] = 0;
}

void FogMethod::activate(MethodVO* vo, IContext* context)
{
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	data[index] = m_colorR;
	data[index + 1] = m_colorG;
	data[index + 2] = m_colorB;
	data[index + 4] = m_minDistance;
	data[index + 5] = 1 / (m_maxDistance - m_minDistance);
}

void FogMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int fogColor = regCache->getFreeFragmentConstant();
	unsigned int fogData = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(fogColor) * 4;

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	code.sub(temp ^ Regs::w, m_sharedRegisters->m_projectivePositionVarying ^ Regs::z, fogData ^ Regs::x); // z - minDistance
	code.mul(temp ^ Regs::w, temp ^ Regs::w, fogData ^ Regs::y); // ratio = (z - minDistance) / (maxDistance - minDistance)
	code.sat(temp ^ Regs::w, temp);
	code.sub(temp ^ Regs::xyz, fogColor, targetReg); // fogColor - targetReg
	code.mul(temp ^ Regs::xyz, temp, temp ^ Regs::w); // (fogColor - targetColor) * ratio
	code.add(targetReg ^ Regs::xyz, targetReg, temp); // targetColor + (fogColor - targetColor) * ratio
}