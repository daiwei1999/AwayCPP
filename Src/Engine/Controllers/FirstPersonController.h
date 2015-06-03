#pragma once

#include "Common.h"
#include "ControllerBase.h"

AWAY_NAMESPACE_BEGIN

class FirstPersonController : public ControllerBase
{
public:
	FirstPersonController(Entity* targetObject = nullptr, float panAngle = 0, float tiltAngle = 90, float minTiltAngle = -90, float maxTiltAngle = 90, int steps = 8, bool wrapPanAngle = false);

	int getSteps() { return m_steps; }
	void setSteps(int value);
	float getPanAngle() { return m_panAngle; }
	void setPanAngle(float value);
	float getTiltAngle() { return m_tiltAngle; }
	void setTiltAngle(float value);
	float getMinTiltAngle() { return m_minTiltAngle; }
	void setMinTiltAngle(float value);
	float getMaxTiltAngle() { return m_maxTiltAngle; }
	void setMaxTiltAngle(float value);
	bool getWrapPanAngle() { return m_wrapPanAngle; }
	void setWrapPanAngle(bool value);

	void update(bool interpolate = true) override;
	void incrementWalk(float value);
	void incrementStrafe(float value);

private:
	float m_panAngle;
	float m_currentPanAngle;
	float m_tiltAngle;
	float m_currentTiltAngle;
	float m_minTiltAngle;
	float m_maxTiltAngle;
	int m_steps;
	float m_walkIncrement;
	float m_strafeIncrement;
	bool m_wrapPanAngle;

public:
	bool m_fly;
};

AWAY_NAMESPACE_END