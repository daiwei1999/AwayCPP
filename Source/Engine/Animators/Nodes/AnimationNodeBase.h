#pragma once

#include "Common.h"
#include "IAnimationNode.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an abstract base class for nodes in an animation blend tree.
 */
class AnimationNodeBase : public IAnimationNode
{
public:
	AnimationNodeBase();

	std::string& getName() { return m_name; }
	void setName(const std::string& name) { m_name = name; }

private:
	std::string m_name;
};

AWAY_NAMESPACE_END