#include "ColorMatrixFilter.h"
#include "ColorMatrixFilterTask.h"

USING_AWAY_NAMESPACE

ColorMatrixFilter::ColorMatrixFilter(const float matrix[20])
{
	addTask(m_colorMatrixFilterTask = new ColorMatrixFilterTask(matrix));
}