#include "MotionBlurFilter.h"
#include "XFadeCompositeFilterTask.h"
#include "DoubleBufferCopyFilterTask.h"
#include "texture.h"

USING_AWAY_NAMESPACE

MotionBlurFilter::MotionBlurFilter(float strength)
{
	addTask(m_compositeTask = new XFadeCompositeFilterTask(strength));
	addTask(m_copyTask = new DoubleBufferCopyFilterTask());
}

float MotionBlurFilter::getStrength()
{
	return m_compositeTask->getAmount();
}

void MotionBlurFilter::setStrength(float value)
{
	m_compositeTask->setAmount(value);
}

void MotionBlurFilter::update(IContext* context, Camera3D* camera)
{
	m_compositeTask->setOverlayTexture(m_copyTask->getMainInputTexture(context));
	m_compositeTask->setTarget(m_copyTask->getSecondaryInputTexture());
}