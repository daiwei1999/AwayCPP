#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * OutlineMethod provides a shading method to add outlines to an object.
 */
class OutlineMethod : public EffectMethodBase
{
public:
	OutlineMethod(unsigned int color = 0, float thickness = 1, bool showInnerLines = true, bool dedicatedWaterProofMesh = false);

	unsigned int getColor();
	void setColor(unsigned int value);
	float getThickness();
	void setThickness(float value);
	bool getShowInnerLines();
	void setShowInnerLines(bool value);

	void initVO(MethodVO* vo) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	OutlinePass* m_outlinePass;
};

AWAY_NAMESPACE_END