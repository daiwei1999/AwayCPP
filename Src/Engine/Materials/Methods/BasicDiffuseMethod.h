#pragma once

#include "Common.h"
#include "LightingMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * BasicDiffuseMethod provides the default shading method for Lambert (dot3) diffuse lighting.
 */
class BasicDiffuseMethod : public LightingMethodBase
{
public:
	BasicDiffuseMethod();

	bool getUseAmbientTexture() { return m_useAmbientTexture; }
	void setUseAmbientTexture(bool value);
	float getDiffuseAlpha() { return m_diffuseA; }
	void setDiffuseAlpha(float value) { m_diffuseA = value; }
	unsigned int getDiffuseColor() { return m_diffuseColor; }
	void setDiffuseColor(unsigned int value);
	Texture2DBase* getTexture() { return m_texture; }
	void setTexture(Texture2DBase* value);
	float getAlphaThreshold() { return m_alphaThreshold; }
	void setAlphaThreshold(float value);
	void setShadowRegister(unsigned int value) { m_shadowRegister = value; }

	void initVO(MethodVO* vo) override;
	void cleanCompilationData() override;
	void copyFrom(ShadingMethodBase* method) override;
	void getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache) override;
	void getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache) override;
	void getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	void activate(MethodVO* vo, IContext* context) override;

protected:
	void applyShadow(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache);

protected:
	bool m_useTexture;
	bool m_isFirstLight;
	unsigned int m_totalLightColorReg;
	unsigned int m_diffuseInputRegister;
	unsigned int m_shadowRegister;
	float m_alphaThreshold;

private:
	bool m_useAmbientTexture;
	Texture2DBase* m_texture;
	unsigned int m_diffuseColor;
	float m_diffuseR, m_diffuseG, m_diffuseB, m_diffuseA;
};

AWAY_NAMESPACE_END