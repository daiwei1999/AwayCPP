#include "HBlurFilter.h"
#include "HBlurFilterTask.h"

USING_AWAY_NAMESPACE

HBlurFilter::HBlurFilter(float amount, int stepSize)
{
	addTask(m_blurTask = new HBlurFilterTask(amount, stepSize));
}

float HBlurFilter::getAmount()
{
	return m_blurTask->getAmount();
}

void HBlurFilter::setAmount(float value)
{
	m_blurTask->setAmount(value);
}

int HBlurFilter::getStepSize()
{
	return m_blurTask->getStepSize();
}

void HBlurFilter::setStepSize(int value)
{
	m_blurTask->setStepSize(value);
}