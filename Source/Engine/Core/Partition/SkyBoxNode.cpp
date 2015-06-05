#include "SkyBoxNode.h"
#include "SkyBox.h"
#include "PartitionTraverser.h"

USING_AWAY_NAMESPACE

SkyBoxNode::SkyBoxNode(SkyBox* skyBox) : EntityNode(skyBox)
{
	m_skyBox = skyBox;
}

bool SkyBoxNode::isInFrustum(PlaneVector* planes, int numPlanes)
{
	return true;
}

void SkyBoxNode::acceptTraverser(PartitionTraverser* traverser)
{
	if (traverser->enterNode(this))
	{
		EntityNode::acceptTraverser(traverser);
		traverser->applySkyBox(m_skyBox);
	}
}