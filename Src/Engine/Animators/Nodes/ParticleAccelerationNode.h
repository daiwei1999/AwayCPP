#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to apply a constant acceleration vector to the motion of a particle.
 */
class ParticleAccelerationNode : public ParticleNodeBase
{
public:
	ParticleAccelerationNode(ParticlePropertiesMode mode, Vector3D* acceleration = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int ACCELERATION_INDEX;
	static const std::string ACCELERATION_VECTOR3D;

	friend class ParticleAccelerationState;
private:
	Vector3D* m_acceleration;
};

AWAY_NAMESPACE_END