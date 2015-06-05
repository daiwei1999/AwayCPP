#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the color variation of a particle over time.
 */
class ParticleColorNode : public ParticleNodeBase
{
public:
	ParticleColorNode(ParticlePropertiesMode mode, bool usesMultiplier = true, bool usesOffset = true, bool usesCycle = false, bool usesPhase = false,
					  ColorTransform* startColor = nullptr, ColorTransform* endColor = nullptr, float cycleDuration = 1, float cyclePhase = 0);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int START_MULTIPLIER_INDEX;
	static const int DELTA_MULTIPLIER_INDEX;
	static const int START_OFFSET_INDEX;
	static const int DELTA_OFFSET_INDEX;
	static const int CYCLE_INDEX;
	static const std::string COLOR_START_MULTIPLIER;
	static const std::string COLOR_START_OFFSET;
	static const std::string COLOR_END_MULTIPLIER;
	static const std::string COLOR_END_OFFSET;

	friend class ParticleColorState;
private:
	bool m_usesMultiplier;
	bool m_usesOffset;
	bool m_usesCycle;
	bool m_usesPhase;
	ColorTransform* m_startColor;
	ColorTransform* m_endColor;
	float m_cycleDuration;
	float m_cyclePhase;
};

AWAY_NAMESPACE_END