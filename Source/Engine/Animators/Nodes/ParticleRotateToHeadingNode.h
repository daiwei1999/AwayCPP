#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the rotation of a particle to match its heading vector.
 */
class ParticleRotateToHeadingNode : public ParticleNodeBase
{
public:
	ParticleRotateToHeadingNode();

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;

public:
	static const int MATRIX_INDEX;
};

AWAY_NAMESPACE_END