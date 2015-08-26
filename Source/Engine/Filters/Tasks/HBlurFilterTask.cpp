#include "HBlurFilterTask.h"
#include "IContext.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

#define MAX_AUTO_SAMPLES 15

HBlurFilterTask::HBlurFilterTask(float amount, int stepSize)
{
	m_amount = amount;
	setStepSize(stepSize);
}

void HBlurFilterTask::setAmount(float value)
{
	if (value == m_amount)
		return;

	m_amount = value;
	calculateStepSize();
	invalidateProgram();
	updateBlurData();
}

void HBlurFilterTask::setStepSize(int value)
{
	if (value == m_stepSize)
		return;

	m_stepSize = value;
	calculateStepSize();
	invalidateProgram();
	updateBlurData();
}

void HBlurFilterTask::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_data, 1);
}

void HBlurFilterTask::getFragmentCode(Shader& fragmentShader)
{
	int numSamples = 1;

	fragmentShader.mov(Regs::t0, Regs::v0);
	fragmentShader.sub(Regs::t0 ^ Regs::x, Regs::v0 ^ Regs::x, Regs::c0 ^ Regs::x);
	fragmentShader.tex(Regs::t1, Regs::t0, Regs::s0);

	for (float x = m_realStepSize; x <= m_amount; x += m_realStepSize)
	{
		fragmentShader.add(Regs::t0 ^ Regs::x, Regs::t0 ^ Regs::x, Regs::c0 ^ Regs::y);
		fragmentShader.tex(Regs::t2, Regs::t0, Regs::s0);
		fragmentShader.add(Regs::t1, Regs::t1, Regs::t2);
		numSamples++;
	}

	fragmentShader.mul(Regs::oc, Regs::t1, Regs::c0 ^ Regs::z);

	m_data[2] = 1.f / numSamples;
}

void HBlurFilterTask::updateTextures(IContext* context)
{
	FilterTaskBase::updateTextures(context);

	updateBlurData();
}

void HBlurFilterTask::updateBlurData()
{
	float invW = 1.f / m_textureWidth;
	m_data[0] = m_amount * 0.5f * invW;
	m_data[1] = m_realStepSize * invW;
}

void HBlurFilterTask::calculateStepSize()
{
	m_realStepSize = m_stepSize > 0 ? m_stepSize : (m_amount > MAX_AUTO_SAMPLES ? m_amount / MAX_AUTO_SAMPLES : 1);
}