#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * ColorTransformMethod provides a shading method that changes the color of a material analogous to a
 * ColorTransform object.
 */
class ColorTransformMethod : public EffectMethodBase
{
public:
	ColorTransformMethod();

	ColorTransform* getColorTransform() { return m_colorTransform; }
	void setColorTransform(ColorTransform* value) { m_colorTransform = value; }
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	void activate(MethodVO* vo, IContext* context) override;

private:
	ColorTransform* m_colorTransform;
};

AWAY_NAMESPACE_END