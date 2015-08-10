#include "ColorTransformFilter.h"
#include "ColorTransformFilterTask.h"

USING_AWAY_NAMESPACE

ColorTransformFilter::ColorTransformFilter(ColorTransform& colorTransform)
{
	addTask(m_colorTransformFilterTask = new ColorTransformFilterTask(colorTransform));
}

ColorTransform& ColorTransformFilter::getColorTransform()
{
	return m_colorTransformFilterTask->getColorTransform();
}

void ColorTransformFilter::setColorTransform(ColorTransform& value)
{
	m_colorTransformFilterTask->setColorTransform(value);
}