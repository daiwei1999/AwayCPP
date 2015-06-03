#include "MethodDependencyCounter.h"
#include "MethodVO.h"
#include "CompiledPass.h"

USING_AWAY_NAMESPACE

MethodDependencyCounter::MethodDependencyCounter()
{
	m_projectionDependencies = 0;
	m_normalDependencies = 0;
	m_viewDirDependencies = 0;
	m_uvDependencies = 0;
	m_secondaryUVDependencies = 0;
	m_globalPosDependencies = 0;
	m_tangentDependencies = 0;
	m_usesGlobalPosFragment = false;
	m_numPointLights = 0;
	m_lightSourceMask = 0;
}

void MethodDependencyCounter::reset()
{
	m_projectionDependencies = 0;
	m_normalDependencies = 0;
	m_viewDirDependencies = 0;
	m_uvDependencies = 0;
	m_secondaryUVDependencies = 0;
	m_globalPosDependencies = 0;
	m_tangentDependencies = 0;
	m_usesGlobalPosFragment = false;
}

void MethodDependencyCounter::setPositionedLights(int numPointLights, unsigned int lightSourceMask)
{
	m_numPointLights = numPointLights;
	m_lightSourceMask = lightSourceMask;
}

void MethodDependencyCounter::includeMethodVO(MethodVO* methodVO)
{
	if (methodVO->m_needsProjection)
		m_projectionDependencies++;

	if (methodVO->m_needsGlobalVertexPos)
	{
		m_globalPosDependencies++;
		if (methodVO->m_needsGlobalFragmentPos)
			m_usesGlobalPosFragment = true;
	}
	else if (methodVO->m_needsGlobalFragmentPos)
	{
		m_globalPosDependencies++;
		m_usesGlobalPosFragment = true;
	}

	if (methodVO->m_needsNormals)
		m_normalDependencies++;

	if (methodVO->m_needsTangents)
		m_tangentDependencies++;

	if (methodVO->m_needsView)
		m_viewDirDependencies++;

	if (methodVO->m_needsUV)
		m_uvDependencies++;

	if (methodVO->m_needsSecondaryUV)
		m_secondaryUVDependencies++;
}

void MethodDependencyCounter::addWorldSpaceDependencies(bool fragmentLights)
{
	if (m_viewDirDependencies > 0)
		m_globalPosDependencies++;

	if (m_numPointLights > 0 && (m_lightSourceMask & static_cast<unsigned int>(LightSources::LIGHTS)))
	{
		m_globalPosDependencies++;
		if (fragmentLights)
			m_usesGlobalPosFragment = true;
	}
}