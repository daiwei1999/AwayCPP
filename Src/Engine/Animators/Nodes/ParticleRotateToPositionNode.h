#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the rotation of a particle to face to a position
 */
class ParticleRotateToPositionNode : public ParticleNodeBase
{
public:
	ParticleRotateToPositionNode(ParticlePropertiesMode mode, Vector3D* position = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int MATRIX_INDEX;
	static const int POSITION_INDEX;
	static const std::string POSITION_VECTOR3D;

	friend class ParticleRotateToPositionState;
private:
	Vector3D* m_position;
};

AWAY_NAMESPACE_END