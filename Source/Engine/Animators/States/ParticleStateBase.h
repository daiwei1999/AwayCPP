#pragma once

#include "Common.h"
#include "AnimationStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleStateBase : public AnimationStateBase
{
public:
	ParticleStateBase(IAnimator* animator, ParticleNodeBase* particleNode, bool needUpdateTime = false);

	bool needUpdateTime() { return m_needUpdateTime; }
	virtual void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) = 0;

private:
	bool m_needUpdateTime;
};

AWAY_NAMESPACE_END