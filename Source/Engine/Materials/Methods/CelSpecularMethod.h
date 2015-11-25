#pragma once

#include "Common.h"
#include "CompositeSpecularMethod.h"

AWAY_NAMESPACE_BEGIN

/**
 * CelSpecularMethod provides a shading method to add specular cel (cartoon) shading.
 */
class CelSpecularMethod : public CompositeSpecularMethod
{
public:
	CelSpecularMethod(float specularCutOff = 0.5f, BasicSpecularMethod* baseSpecularMethod = nullptr);

	float getSmoothness() { return m_smoothness; }
	void setSmoothness(float value) { m_smoothness = value; }
	float getSpecularCutOff() { return m_specularCutOff; }
	void setSpecularCutOff(float value) { m_specularCutOff = value; }

	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;

private:
	void clampSpecular(ShaderChunk& code, MethodVO* vo, unsigned int t, ShaderRegisterCache* regCache, ShaderRegisterData* sharedRegisters);

private:
	float m_smoothness;
	float m_specularCutOff;
	unsigned int m_dataReg;
};

AWAY_NAMESPACE_END