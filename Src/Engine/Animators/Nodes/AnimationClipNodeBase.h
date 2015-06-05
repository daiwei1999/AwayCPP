#pragma once

#include "Common.h"
#include "AnimationNodeBase.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an abstract base class for nodes with time-based animation data in an animation blend tree.
 */
class AnimationClipNodeBase : public AnimationNodeBase
{
public:
	AnimationClipNodeBase();

	bool getLooping() { return m_looping; }
	void setLooping(bool value);
	bool getStitchFinalFrame() { return m_stitchFinalFrame; }
	void setStitchFinalFrame(bool value);
	unsigned int getTotalDuration();
	Vector3D& getTotalDelta();
	unsigned int getLastFrame();
	std::vector<unsigned int>& getDurations() { return m_durations; }

protected:
	virtual void updateStitch();

public:
	bool m_fixedFrameRate;

protected:
	bool m_looping;
	bool m_stitchDirty;
	bool m_stitchFinalFrame;
	unsigned int m_totalDuration;
	unsigned int m_lastFrame;
	unsigned int m_numFrames;
	std::vector<unsigned int> m_durations;
	Vector3D m_totalDelta;
};

AWAY_NAMESPACE_END