#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleColorState : public ParticleStateBase
{
public:
	ParticleColorState(IAnimator* animator, ParticleColorNode* particleColorNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateColorData();

private:
	Vector3D* m_cycleData;
	Vector3D* m_startMultiplierData;
	Vector3D* m_deltaMultiplierData;
	Vector3D* m_startOffsetData;
	Vector3D* m_deltaOffsetData;
};

AWAY_NAMESPACE_END