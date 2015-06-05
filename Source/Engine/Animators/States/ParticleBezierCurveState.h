#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleBezierCurveState : public ParticleStateBase
{
public:
	ParticleBezierCurveState(IAnimator* animator, ParticleBezierCurveNode* particleBezierCurveNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END