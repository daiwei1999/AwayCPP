#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleSegmentedScaleState : public ParticleStateBase
{
public:
	ParticleSegmentedScaleState(IAnimator* animator, ParticleSegmentedScaleNode* particleSegmentedScaleNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateScaleData();

private:
	std::vector<float> m_scaleData;
};

AWAY_NAMESPACE_END