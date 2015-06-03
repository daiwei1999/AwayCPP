#include "EntityListItemPool.h"

USING_AWAY_NAMESPACE

EntityListItemPool::EntityListItemPool()
{
	m_index = 0;
}

EntityListItemPool::~EntityListItemPool()
{

}

EntityListItem* EntityListItemPool::getItem()
{
	EntityListItem* item;
	if (m_index == m_pool.size())
	{
		item = new EntityListItem();
		m_pool.push_back(item);
		m_index++;
	}
	else
		item = m_pool[m_index++];

	return item;
}

void EntityListItemPool::freeAll()
{
	m_index = 0;
}