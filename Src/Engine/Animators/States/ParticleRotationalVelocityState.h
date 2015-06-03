#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleRotationalVelocityState : public ParticleStateBase
{
public:
	ParticleRotationalVelocityState(IAnimator* animator, ParticleRotationalVelocityNode* particleRotationalVelocityNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateRotationalVelocityData();

private:
	Vector3D* m_rotationalVelocityData;
};

AWAY_NAMESPACE_END