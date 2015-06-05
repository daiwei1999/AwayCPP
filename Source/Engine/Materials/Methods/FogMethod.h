#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * FogMethod provides a method to add distance-based fog to a material.
 */
class FogMethod : public EffectMethodBase
{
public:
	FogMethod(float minDistance, float maxDistance, unsigned int color = 0x808080);

	float getMinDistance() { return m_minDistance; }
	void setMinDistance(float value) { m_minDistance = value; }
	float getMaxDistance() { return m_maxDistance; }
	void setMaxDistance(float value) { m_maxDistance = value; }
	unsigned int getColor() { return m_color; }
	void setColor(unsigned int value);

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	float m_minDistance;
	float m_maxDistance;
	unsigned int m_color;
	float m_colorR, m_colorG, m_colorB;
};

AWAY_NAMESPACE_END