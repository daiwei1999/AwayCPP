#include "AnimatorBase.h"
#include "IAnimationSet.h"
#include "IAnimationState.h"
#include "AnimationNodeBase.h"
#include "AnimationStateBase.h"

USING_AWAY_NAMESPACE

std::list<AnimatorBase*> AnimatorBase::ms_animators;
std::chrono::high_resolution_clock::time_point AnimatorBase::ms_startTime = std::chrono::high_resolution_clock::now();

AnimatorBase::AnimatorBase(IAnimationSet* animationSet)
{
	m_animationSet = animationSet;
	m_absoluteTime = 0;
	m_isPlaying = false;
	m_autoUpdate = true;
	m_time = 0;
	m_playbackSpeed = 1;
}

AnimationStateBase* AnimatorBase::getAnimationState(AnimationNodeBase* node)
{
	auto& state = m_animationStates[node];
	if (!state)
		state = node->createAnimationState(this);

	return state;
}

AnimationNodeBase* AnimatorBase::getActiveAnimation()
{
	return m_animationSet->getAnimation(m_activeAnimationName);
}

void AnimatorBase::setAutoUpdate(bool value)
{
	if (value != m_autoUpdate)
	{
		m_autoUpdate = value;
		if (m_autoUpdate)
			start();
		else
			stop();
	}
}

void AnimatorBase::setTime(float value)
{
	if (value != m_time)
		update(value);
}

void AnimatorBase::start()
{
	if (m_isPlaying || !m_autoUpdate)
		return;

	m_time = 1e-9f * std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - ms_startTime).count();
	m_absoluteTime = m_time;
	m_isPlaying = true;

	auto end = ms_animators.end();
	if (std::find(ms_animators.begin(), end, this) == end)
		ms_animators.push_back(this);
}

void AnimatorBase::stop()
{
	if (!m_isPlaying)
		return;

	m_isPlaying = false;

	auto end = ms_animators.end();
	auto iter = std::find(ms_animators.begin(), end, this);
	if (iter != end)
		ms_animators.erase(iter);
}

void AnimatorBase::resume()
{
	if (m_isPlaying)
		return;

	m_time = 1e-9f * std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - ms_startTime).count();
	m_isPlaying = true;

	auto end = ms_animators.end();
	if (std::find(ms_animators.begin(), end, this) == end)
		ms_animators.push_back(this);
}

void AnimatorBase::phase(float value)
{
	m_activeState->phase(value);
}

void AnimatorBase::update(float time)
{
	updateDeltaTime((time - m_time) * m_playbackSpeed);
	m_time = time;
}

void AnimatorBase::reset(const std::string& name, float offset)
{
	getAnimationState(m_animationSet->getAnimation(name))->offset(offset + m_absoluteTime);
}

void AnimatorBase::addOwner(Mesh* mesh)
{
	m_owners.push_back(mesh);
}

void AnimatorBase::removeOwner(Mesh* mesh)
{
	m_owners.erase(std::find(m_owners.begin(), m_owners.end(), mesh));
}

void AnimatorBase::update()
{
	float time = 1e-9f * std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - ms_startTime).count();
	for (auto animator : ms_animators)
		animator->update(time);
}

void AnimatorBase::updateDeltaTime(float dt)
{
	m_absoluteTime += dt;
	m_activeState->update(m_absoluteTime);
}