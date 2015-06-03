#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticlePositionState : public ParticleStateBase
{
public:
	ParticlePositionState(IAnimator* animator, ParticlePositionNode* particlePositionNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END