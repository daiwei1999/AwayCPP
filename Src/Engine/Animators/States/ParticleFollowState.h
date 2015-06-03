#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleFollowState : public ParticleStateBase
{
public:
	ParticleFollowState(IAnimator* animator, ParticleFollowNode* particleFollowNode);

	ObjectContainer3D* getFollowTarget() { return m_followTarget; }
	void setFollowTarget(ObjectContainer3D* value) { m_followTarget = value; }
	
	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void processPosition(float currentTime, float deltaTime, AnimationSubGeometry* subGeometry);
	void precessRotation(float currentTime, float deltaTime, AnimationSubGeometry* subGeometry);
	void processPositionAndRotation(float currentTime, float deltaTime, AnimationSubGeometry* subGeometry);

private:
	bool m_initFlag;
	Vector3D m_prePos, m_targetPos;
	Vector3D m_preEuler, m_targetEuler;
	ObjectContainer3D* m_followTarget;
};

AWAY_NAMESPACE_END