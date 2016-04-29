#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

class DistortionMethod : public EffectMethodBase
{
public:
	DistortionMethod(Texture2DBase* normalMap, Texture* sceneTexture, float distortUFactor = 0, float distortVFactor = 0, float distortFactor = 0, float alphaFactor = 1);

	float getDistortUFactor() { return m_distortUFactor; }
	void setDistortUFactor(float value) { m_distortUFactor = value; }
	float getDistortVFactor() { return m_distortVFactor; }
	void setDistortVFactor(float value) { m_distortVFactor = value; }
	float getDistortFactor() { return m_distortFactor; }
	void setDistortFactor(float value) { m_distortFactor = value; }
	float getAlphaFactor() { return m_alphaFactor; }
	void setAlphaFactor(float value) { m_alphaFactor = value; }

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void activate(MethodVO* vo, IContext* context) override;
	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	Texture2DBase* m_normalMap;
	Texture* m_sceneTexture;
	float m_distortUFactor;
	float m_distortVFactor;
	float m_distortFactor;
	float m_alphaFactor;
	unsigned int m_distortUVVaring;
};

AWAY_NAMESPACE_END