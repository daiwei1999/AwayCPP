#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleUVState : public ParticleStateBase
{
public:
	ParticleUVState(IAnimator* animator, ParticleUVNode* particleUVNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;
};

AWAY_NAMESPACE_END