#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"
#include "ColorSegmentPoint.h"

AWAY_NAMESPACE_BEGIN

class ParticleSegmentedColorNode : public ParticleNodeBase
{
public:
	ParticleSegmentedColorNode(bool usesMultiplier, bool usesOffset, ColorTransform startColor, ColorTransform endColor, std::vector<ColorSegmentPoint> segmentPoints);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;

public:
	static const int START_MULTIPLIER_INDEX;
	static const int START_OFFSET_INDEX;
	static const int TIME_DATA_INDEX;

	friend class ParticleSegmentedColorState;
private:
	bool m_usesMultiplier;
	bool m_usesOffset;
	ColorTransform m_startColor;
	ColorTransform m_endColor;
	std::vector<ColorSegmentPoint> m_segmentPoints;
};

AWAY_NAMESPACE_END