#pragma once

#include "Common.h"
#include "ShadingMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * ShadowMapMethodBase provides an abstract base method for shadow map methods.
 */
class ShadowMapMethodBase : public ShadingMethodBase
{
public:
	ShadowMapMethodBase(LightBase* castingLight);

	float getAlpha() { return m_alpha; }
	void setAlpha(float value) { m_alpha = value; }
	LightBase* getCastingLight() { return m_castingLight; }
	float getEpsilon() { return m_epsilon; }
	void setEpsilon(float value) { m_epsilon = value; }

	virtual void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) = 0;

protected:
	LightBase* m_castingLight;
	ShadowMapperBase* m_shadowMapper;
	float m_epsilon;
	float m_alpha;
};

AWAY_NAMESPACE_END