#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class EntityListItem
{
public:
	Entity* m_entity;
	EntityListItem* m_next;
};

class EntityListItemPool
{
public:
	EntityListItemPool();
	~EntityListItemPool();

	EntityListItem* getItem();
	void freeAll();

private:
	unsigned int m_index;
	std::vector<EntityListItem*> m_pool;
};

AWAY_NAMESPACE_END