#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to set the starting velocity of a particle.
 */
class ParticleVelocityNode : public ParticleNodeBase
{
public:
	ParticleVelocityNode(ParticlePropertiesMode mode, Vector3D* velocity = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int VELOCITY_INDEX;
	static const std::string VELOCITY_VECTOR3D;

	friend class ParticleVelocityState;
private:
	Vector3D* m_velocity;
};

AWAY_NAMESPACE_END