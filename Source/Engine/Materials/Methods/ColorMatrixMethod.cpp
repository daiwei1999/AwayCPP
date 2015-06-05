#include "ColorMatrixMethod.h"
#include "MethodVO.h"
#include "ShaderChunk.h"
#include "ShaderRegisterCache.h"

USING_AWAY_NAMESPACE

ColorMatrixMethod::ColorMatrixMethod(const float matrix[20])
{
	std::memcpy(m_matrix, matrix, 20 * sizeof(float));
}

void ColorMatrixMethod::activate(MethodVO* vo, IContext* context)
{
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;

	// r
	data[index] = m_matrix[0];
	data[index + 1] = m_matrix[1];
	data[index + 2] = m_matrix[2];
	data[index + 3] = m_matrix[3];

	// g
	data[index + 4] = m_matrix[5];
	data[index + 5] = m_matrix[6];
	data[index + 6] = m_matrix[7];
	data[index + 7] = m_matrix[8];

	// b
	data[index + 8] = m_matrix[10];
	data[index + 9] = m_matrix[11];
	data[index + 10] = m_matrix[12];
	data[index + 11] = m_matrix[13];

	// a
	data[index + 12] = m_matrix[15];
	data[index + 13] = m_matrix[16];
	data[index + 14] = m_matrix[17];
	data[index + 15] = m_matrix[18];

	// rgba offset
	data[index + 16] = m_matrix[4];
	data[index + 17] = m_matrix[9];
	data[index + 18] = m_matrix[14];
	data[index + 19] = m_matrix[19];
}

void ColorMatrixMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int colorMultReg = regCache->getFreeFragmentConstant();
	regCache->getFreeFragmentConstant();
	regCache->getFreeFragmentConstant();
	regCache->getFreeFragmentConstant();
	unsigned int colorOffsetReg = regCache->getFreeFragmentConstant();
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(colorMultReg) * 4;

	code.m44(targetReg, targetReg, colorMultReg);
	code.add(targetReg, targetReg, colorOffsetReg);
}