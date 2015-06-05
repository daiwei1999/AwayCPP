#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * IMaterialOwner provides an interface for objects that can use materials.
 */
class IMaterialOwner
{
public:
	/**
	 * The material with which to render the object.
	 */
	virtual MaterialBase* getMaterial() = 0;

	/**
	 * The animation used by the material to assemble the vertex code.
	 */
	virtual IAnimator* getAnimator() = 0;
};

AWAY_NAMESPACE_END