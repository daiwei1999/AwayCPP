#pragma once

#include "Common.h"
#include "LookAtController.h"
#include "MathConsts.h"

AWAY_NAMESPACE_BEGIN

/**
 * Extended camera used to hover round a specified target object.
 */
class HoverController : public LookAtController
{
public:
	HoverController(Entity* targetObject = nullptr,
		            float panAngle = 0,
		            float tiltAngle = 90,
		            float distance = 1000,
		            float minTiltAngle = -90,
		            float maxTiltAngle = 90,
		            float minPanAngle = -MathConsts::Infinity,
		            float maxPanAngle = MathConsts::Infinity,
		            int steps = 8,
		            float yFactor = 2,
		            bool wrapPanAngle = false);

	int getSteps() { return m_steps; }
	void setSteps(int value);
	float getPanAngle() { return m_panAngle; }
	void setPanAngle(float value);
	float getTiltAngle() { return m_tiltAngle; }
	void setTiltAngle(float value);
	float getDistance() { return m_distance; }
	void setDistance(float value);
	float getMinPanAngle() { return m_minPanAngle; }
	void setMinPanAngle(float value);
	float getMaxPanAngle() { return m_maxPanAngle; }
	void setMaxPanAngle(float value);
	float getMinTiltAngle() { return m_minTiltAngle; }
	void setMinTiltAngle(float value);
	float getMaxTiltAngle() { return m_maxTiltAngle; }
	void setMaxTiltAngle(float value);
	float getYFactor() { return m_yFactor; }
	void setYFactor(float value);
	bool getWrapPanAngle() { return m_wrapPanAngle; }
	void setWrapPanAngle(bool value);

	void update(bool interpolate = true) override;

private:
	float m_panAngle;
	float m_currentPanAngle;
	float m_tiltAngle;
	float m_currentTiltAngle;
	float m_distance;
	float m_minPanAngle;
	float m_maxPanAngle;
	float m_minTiltAngle;
	float m_maxTiltAngle;
	int m_steps;
	float m_yFactor;
	bool m_wrapPanAngle;
};

AWAY_NAMESPACE_END