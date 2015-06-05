#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * MethodDependencyCounter keeps track of the number of dependencies for "named registers" used across methods.
 * Named registers are that are not necessarily limited to a single method. They are created by the compiler and
 * passed on to methods. The compiler uses the results to reserve usages through RegisterPool, which can be removed
 * each time a method has been compiled into the shader.
 */
class MethodDependencyCounter
{
public:
	MethodDependencyCounter();

	void reset();
	void setPositionedLights(int numPointLights, unsigned int lightSourceMask);
	void includeMethodVO(MethodVO* methodVO);
	bool usesGlobalPosFragment() { return m_usesGlobalPosFragment; }
	int getTangentDependencies() { return m_tangentDependencies; }
	int getProjectionDependencies() { return m_projectionDependencies; }
	int getNormalDependencies() { return m_normalDependencies; }
	int getViewDirDependencies() { return m_viewDirDependencies; }
	int getUVDependencies() { return m_uvDependencies; }
	int getSecondaryUVDependencies() { return m_secondaryUVDependencies; }
	int getGlobalPosDependencies() { return m_globalPosDependencies; }
	void addWorldSpaceDependencies(bool fragmentLights);

private:
	int m_projectionDependencies;
	int m_normalDependencies;
	int m_viewDirDependencies;
	int m_uvDependencies;
	int m_secondaryUVDependencies;
	int m_globalPosDependencies;
	int m_tangentDependencies;
	bool m_usesGlobalPosFragment;
	int m_numPointLights;
	unsigned int m_lightSourceMask;
};

AWAY_NAMESPACE_END