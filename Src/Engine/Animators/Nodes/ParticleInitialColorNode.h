#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleInitialColorNode : public ParticleNodeBase
{
public:
	ParticleInitialColorNode(ParticlePropertiesMode mode, bool usesMultiplier = true, bool usesOffset = false, ColorTransform* initialColor = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int MULTIPLIER_INDEX;
	static const int OFFSET_INDEX;
	static const std::string COLOR_INITIAL_MULTIPLIER;
	static const std::string COLOR_INITIAL_OFFSET;

	friend class ParticleInitialColorState;
private:
	bool m_usesMultiplier;
	bool m_usesOffset;
	ColorTransform* m_initialColor;
};

AWAY_NAMESPACE_END