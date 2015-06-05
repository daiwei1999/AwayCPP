#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleAccelerationState : public ParticleStateBase
{
public:
	ParticleAccelerationState(IAnimator* animator, ParticleAccelerationNode* particleAccelerationNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END