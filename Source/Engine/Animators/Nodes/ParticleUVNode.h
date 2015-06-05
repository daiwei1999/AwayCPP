#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

enum class Axis
{
	U_AXIS,
	V_AXIS
};

enum class Formula
{
	LINEAR_EASE,
	SINE_EASE
};

/**
 * A particle animation node used to control the UV offset and scale of a particle over time.
 */
class ParticleUVNode : public ParticleNodeBase
{
public:
	ParticleUVNode(ParticlePropertiesMode mode, float cycle = 1, float scale = 1, Axis axis = Axis::U_AXIS, Formula formula = Formula::LINEAR_EASE);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int UV_INDEX;
	static const std::string UV_VECTOR3D;

	friend class ParticleUVState;
private:
	float m_cycle;
	float m_scale;
	Axis m_axis;
	Formula m_formula;
};

AWAY_NAMESPACE_END