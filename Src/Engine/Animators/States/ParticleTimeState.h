#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleTimeState : public ParticleStateBase
{
public:
	ParticleTimeState(IAnimator* animator, ParticleTimeNode* particleTimeNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END