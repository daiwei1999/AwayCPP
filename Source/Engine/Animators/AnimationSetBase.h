#pragma once

#include "Common.h"
#include "IAnimationSet.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an abstract base class for data set classes that hold animation data for use in animator classes.
 */
class AnimationSetBase : public IAnimationSet
{
public:
	AnimationSetBase();

	bool usesCPU() override { return m_usesCPU; }
	bool hasAnimation(const std::string& name) override { return m_animationDictionary.count(name) > 0; }
	AnimationNodeBase* getAnimation(const std::string& name) override { return m_animationDictionary[name]; }
	virtual void addAnimation(AnimationNodeBase* node);
	void resetGPUCompatibility() override { m_usesCPU = false; }
	void cancelGPUCompatibility() override { m_usesCPU = true; }
	std::vector<AnimationNodeBase*> getAnimations() { return m_animations; }
	std::vector<std::string>& getAnimationNames() { return m_animationNames; }

protected:
	unsigned int findTempReg(std::vector<unsigned int>& excludes, std::initializer_list<unsigned int> otherExcludes);

private:
	bool m_usesCPU;
	std::vector<AnimationNodeBase*> m_animations;
	std::vector<std::string> m_animationNames;
	std::unordered_map<std::string, AnimationNodeBase*> m_animationDictionary;
};

AWAY_NAMESPACE_END