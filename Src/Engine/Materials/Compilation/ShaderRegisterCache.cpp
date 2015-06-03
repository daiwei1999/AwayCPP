#include "ShaderRegisterCache.h"

USING_AWAY_NAMESPACE

ShaderRegisterCache::ShaderRegisterCache() : m_vertexAttributesCache(RegisterType::A, 8),
											 m_vertexConstantsCache(RegisterType::C, 128),
											 m_fragmentConstantsCache(RegisterType::C, 28),
											 m_vertexTempCache(RegisterType::T, 8, false),
											 m_fragmentTempCache(RegisterType::T, 8, false),
											 m_varyingCache(RegisterType::V, 8),
											 m_textureCache(RegisterType::S, 8)
{
	m_vertexAttributesOffset = 0;
	m_vertexConstantOffset = 0;
	m_varyingsOffset = 0;
	m_fragmentConstantOffset = 0;
}

void ShaderRegisterCache::reset()
{
	m_vertexAttributesCache.reset();
	m_vertexConstantsCache.reset();
	m_fragmentConstantsCache.reset();
	m_vertexTempCache.reset();
	m_fragmentTempCache.reset();
	m_varyingCache.reset();
	m_textureCache.reset();
	
	m_numUsedVertexConstants = 0;
	m_numUsedStreams = 0;
	m_numUsedTextures = 0;
	m_numUsedVaryings = 0;
	m_numUsedFragmentConstants = 0;

	for (int i = 0; i < m_vertexAttributesOffset; i++)
		getFreeVertexAttribute();

	for (int i = 0; i < m_vertexConstantOffset; i++)
		getFreeVertexConstant();

	for (int i = 0; i < m_varyingsOffset; i++)
		getFreeVarying();

	for (int i = 0; i < m_fragmentConstantOffset; i++)
		getFreeFragmentConstant();
}

void ShaderRegisterCache::addFragmentTempUsages(unsigned int value, int usageCount)
{
	m_fragmentTempCache.addUsage(value, usageCount);
}

void ShaderRegisterCache::removeFragmentTempUsage(unsigned int value)
{
	m_fragmentTempCache.removeUsage(value);
}

void ShaderRegisterCache::addVertexTempUsages(unsigned int value, int usageCount)
{
	m_vertexTempCache.addUsage(value, usageCount);
}

void ShaderRegisterCache::removeVertexTempUsage(unsigned int value)
{
	m_vertexTempCache.removeUsage(value);
}

unsigned int ShaderRegisterCache::getFreeFragmentVectorTemp()
{
	return m_fragmentTempCache.requestFreeVectorReg();
}

unsigned int ShaderRegisterCache::getFreeFragmentSingleTemp()
{
	return m_fragmentTempCache.requestFreeRegComponent();
}

unsigned int ShaderRegisterCache::getFreeVarying()
{
	unsigned int result = m_varyingCache.requestFreeVectorReg();
	++m_numUsedVaryings;
	return result;
}

unsigned int ShaderRegisterCache::getFreeFragmentConstant()
{
	unsigned int result = m_fragmentConstantsCache.requestFreeVectorReg();
	++m_numUsedFragmentConstants;
	return result;
}

unsigned int ShaderRegisterCache::getFreeVertexConstant()
{
	unsigned int result = m_vertexConstantsCache.requestFreeVectorReg();
	++m_numUsedVertexConstants;
	return result;
}

unsigned int ShaderRegisterCache::getFreeVertexVectorTemp()
{
	return m_vertexTempCache.requestFreeVectorReg();
}

unsigned int ShaderRegisterCache::getFreeVertexSingleTemp()
{
	return m_vertexTempCache.requestFreeRegComponent();
}

unsigned int ShaderRegisterCache::getFreeVertexAttribute()
{
	unsigned int result = m_vertexAttributesCache.requestFreeVectorReg();
	++m_numUsedStreams;
	return result;
}

unsigned int ShaderRegisterCache::getFreeTextureReg()
{
	unsigned int result = m_textureCache.requestFreeVectorReg();
	++m_numUsedTextures;
	return result;
}