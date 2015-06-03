#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to control the position of a particle over time along a bezier curve.
 */
class ParticleBezierCurveNode : public ParticleNodeBase
{
public:
	ParticleBezierCurveNode(ParticlePropertiesMode mode, Vector3D* controlPoint = nullptr, Vector3D* endPoint = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void generatePropertyOfOneParticle(ParticleProperties& param) override;

public:
	static const int BEZIER_CONTROL_INDEX;
	static const int BEZIER_END_INDEX;
	static const std::string BEZIER_CONTROL_VECTOR3D;
	static const std::string BEZIER_END_VECTOR3D;

	friend class ParticleBezierCurveState;
private:
	Vector3D* m_controlPoint;
	Vector3D* m_endPoint;
};

AWAY_NAMESPACE_END