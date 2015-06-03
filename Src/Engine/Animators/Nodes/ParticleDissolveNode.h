#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleDissolveNode : public ParticleNodeBase
{
public:
	ParticleDissolveNode(float speed, float timeOffset);

	void getAGALFragmentCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;

public:
	static const int DISSOLVE_INDEX;

	friend class ParticleDissolveState;
private:
	float m_speed;
	float m_timeOffset;
};

AWAY_NAMESPACE_END