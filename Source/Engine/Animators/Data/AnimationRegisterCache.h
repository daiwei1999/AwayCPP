#pragma once

#include "Common.h"
#include "ShaderRegisterCache.h"

AWAY_NAMESPACE_BEGIN

class AnimationRegisterCache : public ShaderRegisterCache
{
public:
	AnimationRegisterCache();

	void reset() override;
	void setUVSourceAndTarget(unsigned int uvAttribute, unsigned int uvVaring);
	void setRegisterIndex(AnimationNodeBase* node, int parameterIndex, int registerIndex);
	int getRegisterIndex(AnimationNodeBase* node, int parameterIndex);
	void getInitCode(ShaderChunk& code);
	void getCombinationCode(ShaderChunk& code);
	void initColorRegisters(ShaderChunk& code);
	void getColorPassCode(ShaderChunk& code);
	void getColorCombinationCode(ShaderChunk& code, unsigned int shadedTarget);

	int getVertexConstantCount() { return m_numVertexConstant; }
	int getFragmentConstantCount() { return m_numFragmentConstant; }
	void setDataLength();
	void setVertexConst(int index, float x = 0, float y = 0, float z = 0, float w = 0);
	void setVertexConst(int index, std::vector<float>& data);
	void setVertexConst(int index, Matrix3D& matrix);
	void setFragmentConst(int index, float x = 0, float y = 0, float z = 0, float w = 0);
	void setFragmentConst(int index, std::vector<float>& data);

public:
	unsigned int m_uvAttribute;
	unsigned int m_positionTarget;
	unsigned int m_scaleAndRotateTarget;
	unsigned int m_velocityTarget;
	unsigned int m_vertexTime;
	unsigned int m_vertexLife;
	unsigned int m_vertexZeroConst;
	unsigned int m_vertexOneConst;
	unsigned int m_vertexTwoConst;
	unsigned int m_uvTarget;
	unsigned int m_colorAddTarget;
	unsigned int m_colorMulTarget;
	unsigned int m_colorAddVarying;
	unsigned int m_colorMulVarying;
	unsigned int m_colorTarget;
	unsigned int m_uvVaring;
	std::vector<unsigned int> m_rotationRegisters;

	std::vector<unsigned int> m_sourceRegisters;
	std::vector<unsigned int> m_targetRegisters;
	std::unordered_map<AnimationNodeBase*, std::vector<int>> m_indexDictionary;

	bool m_needFragmentAnimation;
	bool m_needUVAnimation;
	bool m_hasUVNode;
	bool m_needVelocity;
	bool m_hasBillboard;
	bool m_hasColorMulNode;
	bool m_hasColorAddNode;

	std::vector<float> m_vertexConstantData;
	std::vector<float> m_fragmentConstantData;

private:
	int m_numVertexConstant;
	int m_numFragmentConstant;
};

AWAY_NAMESPACE_END