#pragma once

#include "Common.h"
#include "LightingMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * BasicSpecularMethod provides the default shading method for Blinn-Phong specular highlights (an optimized but approximated
 * version of Phong specularity).
 */
class BasicSpecularMethod : public LightingMethodBase
{
public:
	BasicSpecularMethod();

	float getGloss() { return m_gloss; }
	void setGloss(float value) { m_gloss = value; }
	float getSpecular() { return m_specular; }
	void setSpecular(float value);
	unsigned int getSpecularColor() { return m_specularColor; }
	void setSpecularColor(unsigned int value);
	Texture2DBase* getTexture() { return m_texture; }
	void setTexture(Texture2DBase* value);
	void setShadowRegister(unsigned int value) { m_shadowRegister = value; }

	void initVO(MethodVO* vo) override;
	void cleanCompilationData() override;
	void copyFrom(ShadingMethodBase* method) override;
	void getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache) override;
	void getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache) override;
	void getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	void activate(MethodVO* vo, IContext* context) override;

private:
	void updateSpecular();

protected:
	bool m_useTexture;
	bool m_isFirstLight;
	unsigned int m_totalLightColorReg;
	unsigned int m_specularTexData;
	unsigned int m_specularDataRegister;

private:
	Texture2DBase* m_texture;
	float m_gloss;
	float m_specular;
	unsigned int m_specularColor;
	float m_specularR, m_specularG, m_specularB;
	unsigned int m_shadowRegister;
};

AWAY_NAMESPACE_END