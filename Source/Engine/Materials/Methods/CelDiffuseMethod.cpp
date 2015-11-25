#include "CelDiffuseMethod.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "MethodVO.h"
#include "Regs.h"

USING_AWAY_NAMESPACE
using namespace std::placeholders;

CelDiffuseMethod::CelDiffuseMethod(int levels, BasicDiffuseMethod* baseDiffuseMethod) : CompositeDiffuseMethod(std::bind(&CelDiffuseMethod::clampDiffuse, this, _1, _2, _3, _4, _5), baseDiffuseMethod)
{
	m_levels = levels;
	m_smoothness = 0.1f;
}

void CelDiffuseMethod::initConstants(MethodVO* vo)
{
	CompositeDiffuseMethod::initConstants(vo);

	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_secondaryFragmentConstantsIndex;
	data[index + 1] = 1;
	data[index + 2] = 0;
}

void CelDiffuseMethod::cleanCompilationData()
{
	CompositeDiffuseMethod::cleanCompilationData();
}

void CelDiffuseMethod::activate(MethodVO* vo, IContext* context)
{
	CompositeDiffuseMethod::activate(vo, context);
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_secondaryFragmentConstantsIndex;
	data[index] = (float)m_levels;
	data[index + 3] = m_smoothness;
}

void CelDiffuseMethod::getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	m_dataReg = regCache->getFreeFragmentConstant();
	vo->m_secondaryFragmentConstantsIndex = REGISTER_INDEX(m_dataReg) * 4;
	CompositeDiffuseMethod::getFragmentPreLightingCode(code, vo, regCache);
}

void CelDiffuseMethod::clampDiffuse(ShaderChunk& code, MethodVO* vo, unsigned int t, ShaderRegisterCache* regCache, ShaderRegisterData* sharedRegisters)
{
	unsigned int tx = t ^ Regs::x;
	unsigned int ty = t ^ Regs::y;
	unsigned int tz = t ^ Regs::z;
	unsigned int tw = t ^ Regs::w;
	unsigned int dx = m_dataReg ^ Regs::x;
	unsigned int dy = m_dataReg ^ Regs::y;

	code.mul(tw, tw, dx);
	code.frc(tz, tw);
	code.sub(ty, tw, tz);
	code.mov(tx, dx);
	code.sub(tx, tx, dy);
	code.rcp(tx, tx);
	code.mul(tw, ty, tx);

	// previous clamped strength
	code.sub(ty, tw, tx);

	// fract/epsilon (so 0 - epsilon will become 0 - 1)
	code.div(tz, tz, m_dataReg ^ Regs::w);
	code.sat(tz, tz);

	code.mul(tw, tw, tz);
	code.sub(tz, dy, tz);
	code.mul(ty, ty, tz);
	code.add(tw, tw, ty);
	code.sat(tw, tw);
}