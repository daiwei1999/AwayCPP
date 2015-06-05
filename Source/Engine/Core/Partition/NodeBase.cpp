#include "NodeBase.h"
#include "PartitionTraverser.h"
#include "WireframePrimitiveBase.h"

USING_AWAY_NAMESPACE

NodeBase::NodeBase()
{
	m_parent = nullptr;
	m_numChildNodes = 0;
	m_numEntities = 0;
	m_collectionMark = 0;
	m_debugPrimitive = nullptr;
}

void NodeBase::setShowBounds(bool value)
{
	if ((m_debugPrimitive && value) || (!m_debugPrimitive && !value))
		return;

	if (value)
	{ 
		m_debugPrimitive = createDebugBounds();
	}
	else
	{
		delete m_debugPrimitive;
		m_debugPrimitive = nullptr;
	}

	for (auto childNode : m_childNodes)
		childNode->setShowBounds(value);
}

void NodeBase::addNode(NodeBase* node)
{
	node->m_parent = this;
	m_numEntities += node->m_numEntities;
	m_childNodes.push_back(node);
	m_numChildNodes++;

	node->setShowBounds(m_debugPrimitive != nullptr);

	// update numEntities in the tree
	int numEntities = node->m_numEntities;
	node = this;
	do 
	{
		node->m_numEntities += numEntities;
	} while ((node = node->m_parent) != nullptr);
}

void NodeBase::removeNode(NodeBase* node)
{
	// a bit faster than splice(i, 1), works only if order is not important
	// override item to be removed with the last in the list, then remove that last one
	// Also, the "real partition nodes" of the tree will always remain unmoved, first in the list, so if there's
	// an order dependency for them, it's still okay
	auto iter = std::find(m_childNodes.begin(), m_childNodes.end(), node);
	*iter = m_childNodes[--m_numChildNodes];
	m_childNodes.pop_back();

	// update numEntities in the tree
	int numEntities = node->m_numEntities;
	node = this;
	do 
	{
		node->m_numEntities -= numEntities;
	} while ((node = node->m_parent) != nullptr);
}

bool NodeBase::isInFrustum(PlaneVector* planes, int numPlanes)
{
	return true;
}

bool NodeBase::isIntersectingRay(Vector3D* rayPosition, Vector3D* rayDirection)
{
	return true;
}

NodeBase* NodeBase::findPartitionForEntity(Entity* entity)
{
	return this;
}

void NodeBase::acceptTraverser(PartitionTraverser* traverser)
{
	if (m_numEntities == 0 && !m_debugPrimitive)
		return;

	if (traverser->enterNode(this))
	{
		for (auto childNode : m_childNodes)
			childNode->acceptTraverser(traverser);

		if (m_debugPrimitive)
			traverser->applyRenderable(m_debugPrimitive);
	}
}

WireframePrimitiveBase* NodeBase::createDebugBounds()
{
	return nullptr;
}

void NodeBase::updateNumEntities(int value)
{
	int diff = value - m_numEntities;
	NodeBase* node = this;
	do 
	{
		node->m_numEntities += diff;
	} while ((node = node->m_parent) != nullptr);
}