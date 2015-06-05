#pragma once

#include "Common.h"
#include "SinglePassMaterialBase.h"
#include "SuperShaderPass.h"
#include "BasicDiffuseMethod.h"

AWAY_NAMESPACE_BEGIN

/**
 * ColorMaterial is a single-pass material that uses a flat color as the surface's diffuse reflection value.
 */
class ColorMaterial : public SinglePassMaterialBase
{
public:
	ColorMaterial(unsigned int color = 0xcccccc, float alpha = 1);

	float getAlpha() { return m_screenPass->getDiffuseMethod()->getDiffuseAlpha(); }
	void setAlpha(float value);
	unsigned int getColor() { return m_screenPass->getDiffuseMethod()->getDiffuseColor(); }
	void setColor(unsigned int value);

	bool requiresBlending() override;

private:
	float m_diffuseAlpha;
};

AWAY_NAMESPACE_END