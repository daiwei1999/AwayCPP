#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleRotateToPositionState : public ParticleStateBase
{
public:
	ParticleRotateToPositionState(IAnimator* animator, ParticleRotateToPositionNode* particleRotateToPositionNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END