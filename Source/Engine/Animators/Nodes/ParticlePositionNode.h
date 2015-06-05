#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to set the starting position of a particle.
 */
class ParticlePositionNode : public ParticleNodeBase
{
public:
	ParticlePositionNode(ParticlePropertiesMode mode, Vector3D* position = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int POSITION_INDEX;
	static const std::string POSITION_VECTOR3D;

	friend class ParticlePositionState;
private:
	Vector3D* m_position; // The position of the particle used in global mode
};

AWAY_NAMESPACE_END