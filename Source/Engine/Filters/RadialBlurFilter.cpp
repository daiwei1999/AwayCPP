#include "RadialBlurFilter.h"
#include "RadialBlurFilterTask.h"

USING_AWAY_NAMESPACE

RadialBlurFilter::RadialBlurFilter(float intensity, float glowGamma, float blurStart, float blurWidth, float cx, float cy)
{
	addTask(m_blurTask = new RadialBlurFilterTask(intensity, glowGamma, blurStart, blurWidth, cx, cy));
}

float RadialBlurFilter::getIntensity()
{
	return m_blurTask->getIntensity();
}

void RadialBlurFilter::setIntensity(float value)
{
	m_blurTask->setIntensity(value);
}

float RadialBlurFilter::getGlowGamma()
{
	return m_blurTask->getGlowGamma();
}

void RadialBlurFilter::setGlowGamma(float value)
{
	m_blurTask->setGlowGamma(value);
}

float RadialBlurFilter::getBlurStart()
{
	return m_blurTask->getBlurStart();
}

void RadialBlurFilter::setBlurStart(float value)
{
	m_blurTask->setBlurStart(value);
}

float RadialBlurFilter::getBlurWidth()
{
	return m_blurTask->getBlurWidth();
}

void RadialBlurFilter::setBlurWidth(float value)
{
	m_blurTask->setBlurWidth(value);
}

float RadialBlurFilter::getCx()
{
	return m_blurTask->getCx();
}

void RadialBlurFilter::setCx(float value)
{
	m_blurTask->setCx(value);
}

float RadialBlurFilter::getCy()
{
	return m_blurTask->getCy();
}

void RadialBlurFilter::setCy(float value)
{
	m_blurTask->setCy(value);
}