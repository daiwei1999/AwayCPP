#include "HoverController.h"
#include "Entity.h"

USING_AWAY_NAMESPACE

HoverController::HoverController(Entity* targetObject,
	                             float panAngle,
	                             float tiltAngle,
	                             float distance,
	                             float minTiltAngle,
	                             float maxTiltAngle,
	                             float minPanAngle,
	                             float maxPanAngle,
	                             int steps,
	                             float yFactor,
	                             bool wrapPanAngle) : LookAtController(targetObject)
{
	m_panAngle = m_currentPanAngle = 0;
	m_tiltAngle = m_currentTiltAngle = 90;
	m_distance = 1000;
	m_minPanAngle = -MathConsts::Infinity;
	m_maxPanAngle = MathConsts::Infinity;
	m_minTiltAngle = -90;
	m_maxTiltAngle = 90;
	m_steps = 8;
	m_yFactor = 2;
	m_wrapPanAngle = false;

	setDistance(distance);
	setPanAngle(panAngle);
	setTiltAngle(tiltAngle);
	setMinPanAngle(minPanAngle);
	setMaxPanAngle(maxPanAngle);
	setMinTiltAngle(minTiltAngle);
	setMaxTiltAngle(maxTiltAngle);
	setSteps(steps);
	setYFactor(yFactor);
	setWrapPanAngle(wrapPanAngle);

	//values passed in constructor are applied immediately
	m_currentPanAngle = m_panAngle;
	m_currentTiltAngle = m_tiltAngle;
}

void HoverController::setSteps(int value)
{
	if (value < 1)
		value = 1;

	if (value != m_steps)
	{
		m_steps = value;
		notifyUpdate();
	}
}

void HoverController::setPanAngle(float value)
{
	value = std::max(m_minPanAngle, std::min(m_maxPanAngle, value));

	if (value != m_panAngle)
	{
		m_panAngle = value;
		notifyUpdate();
	}
}

void HoverController::setTiltAngle(float value)
{
	value = std::max(m_minTiltAngle, std::min(m_maxTiltAngle, value));

	if (value != m_tiltAngle)
	{
		m_tiltAngle = value;
		notifyUpdate();
	}
}

void HoverController::setDistance(float value)
{
	if (value != m_distance)
	{
		m_distance = value;
		notifyUpdate();
	}
}

void HoverController::setMinPanAngle(float value)
{
	if (value != m_minPanAngle)
	{
		m_minPanAngle = value;
		setPanAngle(std::max(m_minPanAngle, std::min(m_maxPanAngle, m_panAngle)));
	}
}

void HoverController::setMaxPanAngle(float value)
{
	if (value != m_maxPanAngle)
	{
		m_maxPanAngle = value;
		setPanAngle(std::max(m_minPanAngle, std::min(m_maxPanAngle, m_panAngle)));
	}
}

void HoverController::setMinTiltAngle(float value)
{
	if (value != m_minTiltAngle)
	{
		m_minTiltAngle = value;
		setTiltAngle(std::max(m_minTiltAngle, std::min(m_maxTiltAngle, m_tiltAngle)));
	}
}

void HoverController::setMaxTiltAngle(float value)
{
	if (value != m_maxTiltAngle)
	{
		m_maxTiltAngle = value;
		setTiltAngle(std::max(m_minTiltAngle, std::min(m_maxTiltAngle, m_tiltAngle)));
	}
}

void HoverController::setYFactor(float value)
{
	if (value != m_yFactor)
	{
		m_yFactor = value;
		notifyUpdate();
	}
}

void HoverController::setWrapPanAngle(bool value)
{
	if (value != m_wrapPanAngle)
	{
		m_wrapPanAngle = value;
		notifyUpdate();
	}
}

void HoverController::update(bool interpolate)
{
	if (m_tiltAngle != m_currentTiltAngle || m_panAngle != m_currentPanAngle)
	{
		notifyUpdate();

		if (m_wrapPanAngle)
		{
			float temp = std::fmod(m_panAngle, 360.f);
			if (m_panAngle < 0)
			{
				m_currentPanAngle += temp + 360 - m_panAngle;
				m_panAngle = temp + 360;
			}
			else
			{
				m_currentPanAngle += temp - m_panAngle;
				m_panAngle = temp;
			}

			while (m_panAngle - m_currentPanAngle < -180)
				m_currentPanAngle -= 360;

			while (m_panAngle - m_currentPanAngle > 180)
				m_currentPanAngle += 360;
		}

		if (interpolate)
		{
			m_currentTiltAngle += (m_tiltAngle - m_currentTiltAngle) / (m_steps + 1);
			m_currentPanAngle += (m_panAngle - m_currentPanAngle) / (m_steps + 1);
		}
		else
		{
			m_currentPanAngle = m_panAngle;
			m_currentTiltAngle = m_tiltAngle;
		}

		// snap coords if angle differences are close
		if ((std::abs(m_tiltAngle - m_currentTiltAngle) < 0.01f) && (std::abs(m_panAngle - m_currentPanAngle) < 0.01f))
		{
			m_currentTiltAngle = m_tiltAngle;
			m_currentPanAngle = m_panAngle;
		}
	}

	if (!m_targetObject)
		return;

	float panAngleRadian = m_currentPanAngle * MathConsts::DEGREES_TO_RADIANS;
	float tiltAngleRadian = m_currentTiltAngle * MathConsts::DEGREES_TO_RADIANS;
	float cosTiltAngle = std::cos(tiltAngleRadian);
	m_targetObject->setX(m_lookAtPosition.m_x + m_distance * std::sin(panAngleRadian) * cosTiltAngle);
	m_targetObject->setZ(m_lookAtPosition.m_z + m_distance * std::cos(panAngleRadian) * cosTiltAngle);
	m_targetObject->setY(m_lookAtPosition.m_y + m_distance * std::sin(tiltAngleRadian) * m_yFactor);

	LookAtController::update();
}