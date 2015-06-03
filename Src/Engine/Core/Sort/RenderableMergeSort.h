#pragma once

#include "Common.h"
#include "IEntitySorter.h"

AWAY_NAMESPACE_BEGIN

/**
 * RenderableSorter sorts the potentially visible IRenderable objects collected by EntityCollector for optimal
 * rendering performance. Objects are sorted first by material, then by distance to the camera. Opaque objects
 * are sorted front to back, while objects that require blending are sorted back to front, to ensure correct
 * blending.
 */
class RenderableMergeSort : public IEntitySorter
{
public:
	void sort(EntityCollector* collector) override;

private:
	RenderableListItem* mergeSortByDepth(RenderableListItem* head);
	RenderableListItem* mergeSortByMaterial(RenderableListItem* head);
};

AWAY_NAMESPACE_END