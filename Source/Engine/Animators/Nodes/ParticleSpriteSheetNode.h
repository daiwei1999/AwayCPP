#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used when a spritesheet texture is required to animate the particle.
 * NB: to enable use of this node, the <code>repeat</code> property on the material has to be set to true.
 */
class ParticleSpriteSheetNode : public ParticleNodeBase
{
public:
	ParticleSpriteSheetNode(ParticlePropertiesMode mode, bool usesCycle, bool usesPhase, int numColumns = 1, int numRows = 1, float cycleDuration = 1, float cyclePhase = 0);

	void getAGALUVCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int UV_INDEX_0;
	static const int UV_INDEX_1;
	static const std::string UV_VECTOR3D;

	friend class ParticleSpriteSheetState;
private:
	bool m_usesCycle;
	bool m_usesPhase;
	int m_numColumns;
	int m_numRows;
	float m_cycleDuration;
	float m_cyclePhase;
};

AWAY_NAMESPACE_END