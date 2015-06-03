#pragma once

#include "Common.h"
#include "MaterialBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * SegmentMaterial is a material exclusively used to render wireframe objects
 */
class SegmentMaterial : public MaterialBase
{
public:
	SegmentMaterial();

private:
	SegmentPass* m_screenPass;
};

AWAY_NAMESPACE_END