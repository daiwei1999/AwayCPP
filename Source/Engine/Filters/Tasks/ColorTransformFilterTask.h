#pragma once

#include "Common.h"
#include "FilterTaskBase.h"
#include "ColorTransform.h"

AWAY_NAMESPACE_BEGIN

class ColorTransformFilterTask : public FilterTaskBase
{
public:
	ColorTransformFilterTask(ColorTransform& colorTransform);

	ColorTransform& getColorTransform() { return m_colorTransform; }
	void setColorTransform(ColorTransform& value) { m_colorTransform = value; }

	void activate(IContext* context, Camera3D* camera, Texture* depthTexture) override;

protected:
	void getFragmentCode(Shader& fragmentShader) override;

private:
	float m_data[8];
	ColorTransform m_colorTransform;
};

AWAY_NAMESPACE_END