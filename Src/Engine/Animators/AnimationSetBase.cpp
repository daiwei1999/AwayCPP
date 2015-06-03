#include "AnimationSetBase.h"
#include "AnimationNodeBase.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

AnimationSetBase::AnimationSetBase()
{
	m_usesCPU = false;
}

void AnimationSetBase::addAnimation(AnimationNodeBase* node)
{
	std::string& name = node->getName();
	if (m_animationDictionary.count(name) > 0)
		assert(false);

	m_animationDictionary[name] = node;
	m_animations.push_back(node);
	m_animationNames.push_back(name);
}

unsigned int AnimationSetBase::findTempReg(std::vector<unsigned int>& excludes, std::initializer_list<unsigned int> otherExcludes)
{
	unsigned int i = 0, reg;
	while (true)
	{
		reg = Regs::t0 + i;
		if (std::find(excludes.begin(), excludes.end(), reg) == excludes.end())
			if (std::find(otherExcludes.begin(), otherExcludes.end(), reg) == otherExcludes.end())
				return reg;

		i++;
	}

	return 0;
}