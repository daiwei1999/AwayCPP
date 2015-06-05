#pragma once

#include "Common.h"
#include "ParticleStateBase.h"
#include "ColorSegmentPoint.h"

AWAY_NAMESPACE_BEGIN

class ParticleSegmentedColorState : public ParticleStateBase
{
public:
	ParticleSegmentedColorState(IAnimator* animator, ParticleSegmentedColorNode* particleSegmentedColorNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateColorData();

private:
	std::vector<float> m_timeLifeData;
	std::vector<float> m_multiplierData;
	std::vector<float> m_offsetData;
};

AWAY_NAMESPACE_END