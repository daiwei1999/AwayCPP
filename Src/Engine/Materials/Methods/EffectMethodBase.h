#pragma once

#include "Common.h"
#include "ShadingMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * EffectMethodBase forms an abstract base class for shader methods that are not dependent on light sources,
 * and are in essence post-process effects on the materials.
 */
class EffectMethodBase : public ShadingMethodBase
{
public:
	virtual void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) = 0;
};

AWAY_NAMESPACE_END