#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node used to create a follow behaviour on a particle system.
 */
class ParticleFollowNode : public ParticleNodeBase
{
public:
	ParticleFollowNode(bool usesPosition = true, bool usesRotation = true, bool smooth = false);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;

public:
	static const int FOLLOW_POSITION_INDEX;
	static const int FOLLOW_ROTATION_INDEX;

	friend class ParticleFollowState;
private:
	bool m_usesPosition;
	bool m_usesRotation;
	bool m_smooth;
};

AWAY_NAMESPACE_END