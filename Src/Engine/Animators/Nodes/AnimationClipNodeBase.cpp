#include "AnimationClipNodeBase.h"

USING_AWAY_NAMESPACE

AnimationClipNodeBase::AnimationClipNodeBase()
{

}

void AnimationClipNodeBase::setLooping(bool value)
{
	if (value != m_looping)
	{
		m_looping = value;
		m_stitchDirty = true;
	}
}

void AnimationClipNodeBase::setStitchFinalFrame(bool value)
{
	if (value != m_stitchFinalFrame)
	{
		m_stitchFinalFrame = value;
		m_stitchDirty = true;
	}
}

unsigned int AnimationClipNodeBase::getTotalDuration()
{
	if (m_stitchDirty)
		updateStitch();

	return m_totalDuration;
}

Vector3D& AnimationClipNodeBase::getTotalDelta()
{
	if (m_stitchDirty)
		updateStitch();

	return m_totalDelta;
}

unsigned int AnimationClipNodeBase::getLastFrame()
{
	if (m_stitchDirty)
		updateStitch();

	return m_lastFrame;
}

void AnimationClipNodeBase::updateStitch()
{
	m_stitchDirty = false;

	m_lastFrame = (m_looping && m_stitchFinalFrame) ? m_numFrames : m_numFrames - 1;

	m_totalDuration = 0;
	m_totalDelta.m_x = 0;
	m_totalDelta.m_y = 0;
	m_totalDelta.m_z = 0;
}