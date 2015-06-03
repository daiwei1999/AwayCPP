#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * EnvMapMethod provides a material method to perform reflection mapping using cube maps.
 */
class EnvMapMethod : public EffectMethodBase
{
public:
	EnvMapMethod(CubeTextureBase* envMap, float alpha = 1);

	Texture2DBase* getMask() { return m_mask; }
	void setMask(Texture2DBase* value);
	CubeTextureBase* getEnvMap() { return m_envMap; }
	void setEnvMap(CubeTextureBase* value) { m_envMap = value; }
	float getAlpha() { return m_alpha; }
	void setAlpha(float value) { m_alpha = value; }

	void initVO(MethodVO* vo) override;
	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	CubeTextureBase* m_envMap;
	Texture2DBase* m_mask;
	float m_alpha;
};

AWAY_NAMESPACE_END