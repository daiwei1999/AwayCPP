#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used as the base node for timekeeping inside a particle. Automatically added to a particle animation set on instantiation.
 */
class ParticleTimeNode : public ParticleNodeBase
{
public:
	ParticleTimeNode(bool usesDuration = false, bool usesLooping = false, bool usesDelay = false, bool usesRibbon = false);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int TIME_STREAM_INDEX;
	static const int TIME_CONSTANT_INDEX;

private:
	bool m_usesDuration;
	bool m_usesLooping;
	bool m_usesDelay;
	bool m_usesRibbon;
};

AWAY_NAMESPACE_END