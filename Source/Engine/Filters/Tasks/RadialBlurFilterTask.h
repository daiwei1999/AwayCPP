#pragma once

#include "FilterTaskBase.h"

AWAY_NAMESPACE_BEGIN

class RadialBlurFilterTask : public FilterTaskBase
{
public:
	RadialBlurFilterTask(float intensity, float glowGamma, float blurStart, float blurWidth, float cx, float cy);

	float getIntensity() { return m_data[0]; }
	void setIntensity(float value) { m_data[0] = value; }
	float getGlowGamma() { return m_data[1]; }
	void setGlowGamma(float value) { m_data[1] = value; }
	float getBlurStart() { return m_data[2]; }
	void setBlurStart(float value) { m_data[2] = value; }
	float getBlurWidth() { return m_data[3]; }
	void setBlurWidth(float value) { m_data[3] = value; }
	float getCx() { return m_data[4]; }
	void setCx(float value) { m_data[4] = value; }
	float getCy() { return m_data[5]; }
	void setCy(float value) { m_data[5] = value; }

	void activate(IContext* context, Camera3D* camera, Texture* depthTexture) override;

protected:
	void getVertexCode(Shader& vertexShader) override;
	void getFragmentCode(Shader& fragmentShader) override;

private:
	float m_data[12];
};

AWAY_NAMESPACE_END