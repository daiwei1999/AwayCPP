#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to set the starting rotational velocity of a particle.
 */
class ParticleRotationalVelocityNode : public ParticleNodeBase
{
public:
	ParticleRotationalVelocityNode(ParticlePropertiesMode mode, Vector3D* rotationalVelocity = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int ROTATIONALVELOCITY_INDEX;
	static const std::string ROTATIONALVELOCITY_VECTOR3D;

	friend class ParticleRotationalVelocityState;
private:
	Vector3D* m_rotationalVelocity;
};

AWAY_NAMESPACE_END