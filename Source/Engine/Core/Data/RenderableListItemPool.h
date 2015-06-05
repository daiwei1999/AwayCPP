#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class RenderableListItem
{
public:
	RenderableListItem* m_next;
	IRenderable* m_renderable;

	// for faster access while sorting or rendering (cached values)
	int m_materialId;
	int m_renderOrderId;
	float m_zIndex;
	bool m_cascaded;
};

class RenderableListItemPool
{
public:
	RenderableListItemPool();
	~RenderableListItemPool();

	RenderableListItem* getItem();
	void freeAll();

private:
	unsigned int m_index;
	std::vector<RenderableListItem*> m_pool;
};

AWAY_NAMESPACE_END