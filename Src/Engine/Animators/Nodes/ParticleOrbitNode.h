#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the position of a particle over time around a circular orbit.
 */
class ParticleOrbitNode : public ParticleNodeBase
{
public:
	ParticleOrbitNode(ParticlePropertiesMode mode, bool usesEulers = true, bool usesCycle = false, bool usesPhase = false,
					  float radius = 100, float cycleDuration = 1, float cyclePhase = 0, Vector3D* eulers = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int ORBIT_INDEX;
	static const int EULERS_INDEX;
	static const std::string ORBIT_VECTOR3D;

	friend class ParticleOrbitState;
private:
	bool m_usesEulers;
	bool m_usesCycle;
	bool m_usesPhase;
	float m_radius;
	float m_cycleDuration;
	float m_cyclePhase;
	Vector3D* m_eulers;
};

AWAY_NAMESPACE_END