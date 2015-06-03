#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the position of a particle over time using simple harmonic motion.
 */
class ParticleOscillatorNode : public ParticleNodeBase
{
public:
	ParticleOscillatorNode(ParticlePropertiesMode mode, Vector3D* oscillator = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int OSCILLATOR_INDEX;
	static const std::string OSCILLATOR_VECTOR3D;

	friend class ParticleOscillatorState;
private:
	Vector3D* m_oscillator;
};

AWAY_NAMESPACE_END