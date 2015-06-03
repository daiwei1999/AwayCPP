#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleOscillatorState : public ParticleStateBase
{
public:
	ParticleOscillatorState(IAnimator* animator, ParticleOscillatorNode* particleOscillatorNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateOscillatorData();

private:
	Vector3D* m_oscillatorData;
};

AWAY_NAMESPACE_END