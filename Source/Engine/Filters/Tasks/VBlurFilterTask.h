#pragma once

#include "Common.h"
#include "FilterTaskBase.h"

AWAY_NAMESPACE_BEGIN

class VBlurFilterTask : public FilterTaskBase
{
public:
	VBlurFilterTask(float amount, int stepSize = -1);

	float getAmount() { return m_amount; }
	void setAmount(float value);
	int getStepSize() { return m_stepSize; }
	void setStepSize(int value);

	void activate(IContext* context, Camera3D* camera, Texture* depthTexture) override;

protected:
	void getFragmentCode(Shader& fragmentShader) override;
	void updateTextures(IContext* context) override;

private:
	void updateBlurData();
	void calculateStepSize();

private:
	float m_amount;
	int m_stepSize;
	float m_realStepSize;
	float m_data[4];
};

AWAY_NAMESPACE_END