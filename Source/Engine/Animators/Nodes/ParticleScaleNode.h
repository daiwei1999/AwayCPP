#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the scale variation of a particle over time.
 */
class ParticleScaleNode : public ParticleNodeBase
{
public:
	ParticleScaleNode(ParticlePropertiesMode mode, bool usesCycle, bool usesPhase, float minScale = 1, float maxScale = 1, float cycleDuration = 1, float cyclePhase = 0);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int SCALE_INDEX;
	static const std::string SCALE_VECTOR3D;

	friend class ParticleScaleState;
private:
	bool m_usesCycle;
	bool m_usesPhase;
	float m_minScale;
	float m_maxScale;
	float m_cycleDuration;
	float m_cyclePhase;
};

AWAY_NAMESPACE_END