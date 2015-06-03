#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an abstract base class for nodes in an animation blend tree.
 */
class IAnimationNode
{
public:
	virtual AnimationStateBase* createAnimationState(IAnimator* animator) = 0;
};

AWAY_NAMESPACE_END