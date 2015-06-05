#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

enum class RimLightMode
{
	ADD,
	MIX,
	MULTIPLY
};

/**
 * RimLightMethod provides a method to add rim lighting to a material. This adds a glow-like effect to edges of objects.
 */
class RimLightMethod : public EffectMethodBase
{
public:
	RimLightMethod(unsigned int color = 0xffffff, float strength = 0.4f, float power = 2, RimLightMode mode = RimLightMode::MIX);

	RimLightMode getMode() { return m_mode; }
	void setMode(RimLightMode value);
	unsigned int getColor() { return m_color; }
	void setColor(unsigned int value);
	float getStrength() { return m_strength; }
	void setStrength(float value) { m_strength = value; }
	float getPower() { return m_power; }
	void setPower(float value) { m_power = value; }

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	unsigned int m_color;
	RimLightMode m_mode;
	float m_colorR, m_colorG, m_colorB;
	float m_strength;
	float m_power;
};

AWAY_NAMESPACE_END