#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

unsigned int PartitionTraverser::m_collectionMark = 0;

PartitionTraverser::PartitionTraverser()
{
	m_scene = nullptr;
}