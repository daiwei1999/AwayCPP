#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleScaleState : public ParticleStateBase
{
public:
	ParticleScaleState(IAnimator* animator, ParticleScaleNode* particleScaleNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateScaleData();

private:
	Vector3D* m_scaleData;
};

AWAY_NAMESPACE_END