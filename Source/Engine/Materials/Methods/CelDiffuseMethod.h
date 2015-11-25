#pragma once

#include "Common.h"
#include "CompositeDiffuseMethod.h"

AWAY_NAMESPACE_BEGIN

/**
 * CelDiffuseMethod provides a shading method to add diffuse cel (cartoon) shading.
 */
class CelDiffuseMethod : public CompositeDiffuseMethod
{
public:
	CelDiffuseMethod(int levels = 3, BasicDiffuseMethod* baseDiffuseMethod = nullptr);

	int getLevels() { return m_levels; }
	void setLevels(int value) { m_levels = value; }
	float getSmoothness() { return m_smoothness; }
	void setSmoothness(float value) { m_smoothness = value; }

	void initConstants(MethodVO* vo) override;
	void cleanCompilationData() override;
	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;

private:
	void clampDiffuse(ShaderChunk& code, MethodVO* vo, unsigned int t, ShaderRegisterCache* regCache, ShaderRegisterData* sharedRegisters);

private:
	int m_levels;
	float m_smoothness;
	unsigned int m_dataReg;
};

AWAY_NAMESPACE_END