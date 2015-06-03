#include "SegmentMaterial.h"
#include "SegmentPass.h"

USING_AWAY_NAMESPACE

SegmentMaterial::SegmentMaterial()
{
	addPass(m_screenPass = new SegmentPass());
	m_screenPass->setMaterial(this);
}