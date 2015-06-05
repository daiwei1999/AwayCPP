#pragma once

#include "Common.h"
#include "RegisterPool.h"

AWAY_NAMESPACE_BEGIN

#define REGISTER_INDEX(x) ((x) & 0xFFF)

/**
 * ShaderRegister Cache provides the usage management system for all registers during shading compilation.
 */
class ShaderRegisterCache
{
public:
	ShaderRegisterCache();

	virtual void reset();

	void addFragmentTempUsages(unsigned int value, int usageCount);
	void removeFragmentTempUsage(unsigned int value);

	void addVertexTempUsages(unsigned int value, int usageCount);
	void removeVertexTempUsage(unsigned int value);

	unsigned int getFreeFragmentVectorTemp();
	unsigned int getFreeFragmentSingleTemp();
	unsigned int getFreeVarying();
	unsigned int getFreeFragmentConstant();
	unsigned int getFreeVertexConstant();
	unsigned int getFreeVertexVectorTemp();
	unsigned int getFreeVertexSingleTemp();
	unsigned int getFreeVertexAttribute();
	unsigned int getFreeTextureReg();

	int getVertexConstantOffset() { return m_vertexConstantOffset; }
	void setVertexConstantOffset(int value) { m_vertexConstantOffset = value; }
	int getVertexAttributesOffset() { return m_vertexAttributesOffset; }
	void setVertexAttributesOffset(int value) { m_vertexAttributesOffset = value; }
	int getVaryingsOffset() { return m_varyingsOffset; }
	void setVaryingsOffset(int value) { m_varyingsOffset = value; }
	int getFragmentConstantOffset() { return m_fragmentConstantOffset; }
	void setFragmentConstantOffset(int value) { m_fragmentConstantOffset = value; }
	int getUsedVertexConstantCount() { return m_numUsedVertexConstants; }
	int getUsedFragmentConstantCount() { return m_numUsedFragmentConstants; }
	int getUsedStreamCount() { return m_numUsedStreams; }
	int getUsedTextureCount() { return m_numUsedTextures; }
	int getUsedVaryingCount() { return m_numUsedVaryings; }

protected:
	int m_vertexConstantOffset;
	int m_fragmentConstantOffset;
	int m_numUsedVertexConstants;
	int m_numUsedFragmentConstants;

private:
	RegisterPool m_vertexAttributesCache;
	RegisterPool m_vertexConstantsCache;
	RegisterPool m_fragmentConstantsCache;
	RegisterPool m_vertexTempCache;
	RegisterPool m_fragmentTempCache;
	RegisterPool m_varyingCache;
	RegisterPool m_textureCache;
	
	int m_vertexAttributesOffset;
	int m_varyingsOffset;
	int m_numUsedStreams;
	int m_numUsedTextures;
	int m_numUsedVaryings;
};

AWAY_NAMESPACE_END