#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleOrbitState : public ParticleStateBase
{
public:
	ParticleOrbitState(IAnimator* animator, ParticleOrbitNode* particleOrbitNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateOrbitData();

private:
	Vector3D* m_orbitData;
	Matrix3D* m_eulersMatrix;
};

AWAY_NAMESPACE_END