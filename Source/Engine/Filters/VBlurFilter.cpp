#include "VBlurFilter.h"
#include "VBlurFilterTask.h"

USING_AWAY_NAMESPACE

VBlurFilter::VBlurFilter(float amount, int stepSize)
{
	addTask(m_blurTask = new VBlurFilterTask(amount, stepSize));
}

float VBlurFilter::getAmount()
{
	return m_blurTask->getAmount();
}

void VBlurFilter::setAmount(float value)
{
	m_blurTask->setAmount(value);
}

int VBlurFilter::getStepSize()
{
	return m_blurTask->getStepSize();
}

void VBlurFilter::setStepSize(int value)
{
	m_blurTask->setStepSize(value);
}