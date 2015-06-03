#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleRotateToHeadingState : public ParticleStateBase
{
public:
	ParticleRotateToHeadingState(IAnimator* animator, ParticleRotateToHeadingNode* particleRotateToHeadingNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END