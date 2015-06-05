#include "RenderableListItemPool.h"

USING_AWAY_NAMESPACE

RenderableListItemPool::RenderableListItemPool()
{
	m_index = 0;
}

RenderableListItemPool::~RenderableListItemPool()
{

}

RenderableListItem* RenderableListItemPool::getItem()
{
	RenderableListItem* item;
	if (m_index == m_pool.size())
	{
		item = new RenderableListItem();
		m_pool.push_back(item);
		m_index++;
	}
	else
		item = m_pool[m_index++];

	return item;
}

void RenderableListItemPool::freeAll()
{
	m_index = 0;
}