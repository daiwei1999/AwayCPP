#include "RimLightMethod.h"
#include "MethodVO.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ShaderRegisterData.h"

USING_AWAY_NAMESPACE

RimLightMethod::RimLightMethod(unsigned int color, float strength, float power, RimLightMode mode)
{
	setColor(color);
	m_strength = strength;
	m_power = power;
	m_mode = mode;
}

void RimLightMethod::setMode(RimLightMode value)
{
	if (value != m_mode)
	{
		m_mode = value;
		invalidateShaderProgram();
	}
}

void RimLightMethod::setColor(unsigned int value)
{
	m_color = value;
	m_colorR = ((value >> 16) & 0xff) / 255.f;
	m_colorG = ((value >> 8) & 0xff) / 255.f;
	m_colorB = (value & 0xff) / 255.f;
}

void RimLightMethod::initVO(MethodVO* vo)
{
	vo->m_needsNormals = true;
	vo->m_needsView = true;
}

void RimLightMethod::initConstants(MethodVO* vo)
{
	(*vo->m_fragmentData)[vo->m_fragmentConstantsIndex + 3] = 1;
}

void RimLightMethod::activate(MethodVO* vo, IContext* context)
{
	std::vector<float>& fragmentData = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	fragmentData[index] = m_colorR;
	fragmentData[index + 1] = m_colorG;
	fragmentData[index + 2] = m_colorB;
	fragmentData[index + 4] = m_strength;
	fragmentData[index + 5] = m_power;
}

void RimLightMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int colorReg = regCache->getFreeFragmentConstant();
	unsigned int dataReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(colorReg) * 4;

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	code.dp3(temp ^ Regs::w, m_sharedRegisters->m_viewDirFragment, m_sharedRegisters->m_normalFragment);
	code.sat(temp ^ Regs::w, temp);
	code.sub(temp ^ Regs::w, colorReg ^ Regs::w, temp ^ Regs::w);
	code.pow(temp ^ Regs::w, temp ^ Regs::w, dataReg ^ Regs::y);
	code.mul(temp ^ Regs::w, temp ^ Regs::w, dataReg ^ Regs::x);
	
	switch (m_mode)
	{
	case RimLightMode::ADD:
		code.mul(temp ^ Regs::xyz, colorReg, temp ^ Regs::w);
		code.add(targetReg ^ Regs::xyz, targetReg, temp);
		break;
	case RimLightMode::MULTIPLY:
		code.mul(temp ^ Regs::xyz, colorReg, temp ^ Regs::w);
		code.mul(targetReg ^ Regs::xyz, targetReg, temp);
		break;
	case RimLightMode::MIX:
		code.sub(temp ^ Regs::xyz, colorReg, targetReg);
		code.mul(temp ^ Regs::xyz, temp, temp ^ Regs::w);
		code.add(targetReg ^ Regs::xyz, targetReg, temp);
		break;
	}
}