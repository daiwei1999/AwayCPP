#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleInitialColorState : public ParticleStateBase
{
public:
	ParticleInitialColorState(IAnimator* animator, ParticleInitialColorNode* particleInitialColorNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateColorData();

private:
	Vector3D* m_multiplierData;
	Vector3D* m_offsetData;
};

AWAY_NAMESPACE_END