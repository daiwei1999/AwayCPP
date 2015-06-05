#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * ColorMatrixMethod provides a shading method that changes the color of a material analogous to a ColorMatrixFilter.
 */
class ColorMatrixMethod : public EffectMethodBase
{
public:
	ColorMatrixMethod(const float matrix[20]);

	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	float m_matrix[20];
};

AWAY_NAMESPACE_END