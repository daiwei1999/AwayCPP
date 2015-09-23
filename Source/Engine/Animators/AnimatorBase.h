#pragma once

#include "Common.h"
#include "IAnimator.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an abstract base class for animator classes that control animation output from a data set subtype of <code>AnimationSetBase</code>.
 */
class AnimatorBase : public IAnimator
{
public:
	AnimatorBase(IAnimationSet* animationSet);

	AnimationStateBase* getAnimationState(AnimationNodeBase* node);
	float getAbsoluteTime() { return m_absoluteTime; }
	IAnimationSet* getAnimationSet() override { return m_animationSet; }
	IAnimationState* getActiveState() { return m_activeState; }
	AnimationNodeBase* getActiveAnimation();
	std::string& getActiveAnimationName() { return m_activeAnimationName; }
	bool getAutoUpdate() { return m_autoUpdate; }
	void setAutoUpdate(bool value);
	float getPlaybackSpeed() { return m_playbackSpeed; }
	void setPlaybackSpeed(float value) { m_playbackSpeed = value; }
	unsigned int getTime() { return m_time; }
	void setTime(unsigned int value);

	virtual void start();
	void stop();
	void phase(float value);
	void update(unsigned int time);
	void reset(const std::string& name, float offset = 0);

	void addOwner(Mesh* mesh) override;
	void removeOwner(Mesh* mesh) override;

	static void update();

protected:
	virtual void updateDeltaTime(float dt);

protected:
	IAnimationSet* m_animationSet;
	std::vector<Mesh*> m_owners;
	AnimationNodeBase* m_activeNode;
	IAnimationState* m_activeState;
	std::string m_activeAnimationName;
	float m_absoluteTime;

private:
	bool m_isPlaying;
	bool m_autoUpdate;
	unsigned int m_time;
	float m_playbackSpeed;
	std::unordered_map<AnimationNodeBase*, AnimationStateBase*> m_animationStates;

	static std::list<AnimatorBase*> ms_animators;
	static std::chrono::high_resolution_clock::time_point ms_startTime;
};

AWAY_NAMESPACE_END