#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "Matrix3D.h"

USING_AWAY_NAMESPACE

AnimationRegisterCache::AnimationRegisterCache()
{

}

void AnimationRegisterCache::reset()
{
	ShaderRegisterCache::reset();

	m_scaleAndRotateTarget = m_targetRegisters[0];
	addVertexTempUsages(m_scaleAndRotateTarget, 1);

	int len = m_targetRegisters.size();
	for (int i = 1; i < len; i++)
	{
		m_rotationRegisters.push_back(m_targetRegisters[i]);
		addVertexTempUsages(m_rotationRegisters[i - 1], 1);
	}

	// constant register
	unsigned int vertexCommonConst = getFreeVertexConstant();
	m_vertexZeroConst = vertexCommonConst ^ Regs::x;
	m_vertexOneConst = vertexCommonConst ^ Regs::y;
	m_vertexTwoConst = vertexCommonConst ^ Regs::z;

	// temporary register
	m_positionTarget = getFreeVertexVectorTemp();
	addVertexTempUsages(m_positionTarget, 1);

	if (m_needVelocity)
	{
		m_velocityTarget = getFreeVertexVectorTemp();
		addVertexTempUsages(m_velocityTarget, 1);
		m_vertexTime = m_velocityTarget ^ Regs::w;
		m_vertexLife = m_positionTarget ^ Regs::w;
	}
	else
	{
		unsigned int tempTime = getFreeVertexVectorTemp();
		addVertexTempUsages(tempTime, 1);
		m_vertexTime = tempTime ^ Regs::x;
		m_vertexLife = tempTime ^ Regs::y;
	}
}

void AnimationRegisterCache::setUVSourceAndTarget(unsigned int uvAttribute, unsigned int uvVaring)
{
	m_uvVaring = uvVaring;
	m_uvAttribute = uvAttribute;

	// uv action is processed after normal actions, so use offsetTarget as uvTarget
	m_uvTarget = m_positionTarget;
}

void AnimationRegisterCache::setRegisterIndex(AnimationNodeBase* node, int parameterIndex, int registerIndex)
{
	if (m_indexDictionary.count(node) == 0)
		m_indexDictionary[node] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	m_indexDictionary[node][parameterIndex] = registerIndex;
}

int AnimationRegisterCache::getRegisterIndex(AnimationNodeBase* node, int parameterIndex)
{
	return m_indexDictionary[node][parameterIndex];
}

void AnimationRegisterCache::getInitCode(ShaderChunk& code)
{
	int len = m_sourceRegisters.size();
	for (int i = 0; i < len; i++)
		code.mov(m_targetRegisters[i], m_sourceRegisters[i]);

	code.mov(m_positionTarget ^ Regs::xyz, m_vertexZeroConst);

	if (m_needVelocity)
		code.mov(m_velocityTarget ^ Regs::xyz, m_vertexZeroConst);
}

void AnimationRegisterCache::getCombinationCode(ShaderChunk& code)
{
	code.add(m_scaleAndRotateTarget ^ Regs::xyz, m_scaleAndRotateTarget, m_positionTarget);
}

void AnimationRegisterCache::initColorRegisters(ShaderChunk& code)
{
	if (m_hasColorMulNode)
	{
		m_colorMulTarget = getFreeVertexVectorTemp();
		addVertexTempUsages(m_colorMulTarget, 1);

		m_colorMulVarying = getFreeVarying();
		code.mov(m_colorMulTarget, m_vertexOneConst);
	}
	if (m_hasColorAddNode)
	{
		m_colorAddTarget = getFreeVertexVectorTemp();
		addVertexTempUsages(m_colorAddTarget, 1);

		m_colorAddVarying = getFreeVarying();
		code.mov(m_colorAddTarget, m_vertexZeroConst);
	}
}

void AnimationRegisterCache::getColorPassCode(ShaderChunk& code)
{
	if (m_needFragmentAnimation && (m_hasColorAddNode || m_hasColorMulNode))
	{
		if (m_hasColorMulNode)
			code.mov(m_colorMulVarying, m_colorMulTarget);

		if (m_hasColorAddNode)
			code.mov(m_colorAddVarying, m_colorAddTarget);
	}
}

void AnimationRegisterCache::getColorCombinationCode(ShaderChunk& code, unsigned int shadedTarget)
{
	m_colorTarget = shadedTarget;
	addFragmentTempUsages(m_colorTarget, 1);

	if (m_needFragmentAnimation && (m_hasColorAddNode || m_hasColorMulNode))
	{
		if (m_hasColorMulNode)
			code.mul(m_colorTarget, m_colorTarget, m_colorMulVarying);

		if (m_hasColorAddNode)
			code.add(m_colorTarget, m_colorTarget, m_colorAddVarying);
	}
}

void AnimationRegisterCache::setDataLength()
{
	m_numVertexConstant = m_numUsedVertexConstants - m_vertexConstantOffset;
	m_numFragmentConstant = m_numUsedFragmentConstants - m_fragmentConstantOffset;
	m_vertexConstantData.resize(m_numVertexConstant * 4);
	m_fragmentConstantData.resize(m_numFragmentConstant * 4);
}

void AnimationRegisterCache::setVertexConst(int index, float x, float y, float z, float w)
{
	index = (index - m_vertexConstantOffset) * 4;
	m_vertexConstantData[index++] = x;
	m_vertexConstantData[index++] = y;
	m_vertexConstantData[index++] = z;
	m_vertexConstantData[index] = w;
}

void AnimationRegisterCache::setVertexConst(int index, std::vector<float>& data)
{
	index = (index - m_vertexConstantOffset) * 4;
	for (float value : data)
		m_vertexConstantData[index++] = value;
}

void AnimationRegisterCache::setVertexConst(int index, Matrix3D& matrix)
{
	matrix.copyRawDataTo(m_vertexConstantData.data(), (index - m_vertexConstantOffset) * 4, true);
}

void AnimationRegisterCache::setFragmentConst(int index, float x, float y, float z, float w)
{
	index = (index - m_fragmentConstantOffset) * 4;
	m_fragmentConstantData[index++] = x;
	m_fragmentConstantData[index++] = y;
	m_fragmentConstantData[index++] = z;
	m_fragmentConstantData[index] = w;
}

void AnimationRegisterCache::setFragmentConst(int index, std::vector<float>& data)
{
	index = (index - m_fragmentConstantOffset) * 4;
	for (float value : data)
		m_fragmentConstantData[index++] = value;
}