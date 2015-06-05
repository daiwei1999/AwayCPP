#include "ColorTransformMethod.h"
#include "MethodVO.h"
#include "ShaderChunk.h"
#include "ShaderRegisterCache.h"
#include "ColorTransform.h"

USING_AWAY_NAMESPACE

ColorTransformMethod::ColorTransformMethod()
{
	m_colorTransform = nullptr;
}

void ColorTransformMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int colorMultReg = regCache->getFreeFragmentConstant();
	unsigned int colorOffsReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(colorMultReg) * 4;

	code.mul(targetReg, targetReg, colorMultReg);
	code.add(targetReg, targetReg, colorOffsReg);
}

void ColorTransformMethod::activate(MethodVO* vo, IContext* context)
{
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	data[index] = m_colorTransform->m_redMultiplier;
	data[index + 1] = m_colorTransform->m_greenMultiplier;
	data[index + 2] = m_colorTransform->m_blueMultiplier;
	data[index + 3] = m_colorTransform->m_alphaMultiplier;
	data[index + 4] = m_colorTransform->m_redOffset / 255;
	data[index + 5] = m_colorTransform->m_greenOffset / 255;
	data[index + 6] = m_colorTransform->m_blueOffset / 255;
	data[index + 7] = m_colorTransform->m_alphaOffset / 255;
}