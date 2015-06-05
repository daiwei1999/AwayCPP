#include "FirstPersonController.h"
#include "Entity.h"

USING_AWAY_NAMESPACE

FirstPersonController::FirstPersonController(Entity* targetObject, float panAngle, float tiltAngle, float minTiltAngle, float maxTiltAngle, int steps, bool wrapPanAngle) : ControllerBase(targetObject)
{
	m_panAngle = m_currentPanAngle = 0;
	m_tiltAngle = m_currentTiltAngle = 90;
	m_minTiltAngle = -90;
	m_maxTiltAngle = 90;
	m_steps = 8;
	m_walkIncrement = 0;
	m_strafeIncrement = 0;
	m_wrapPanAngle = false;
	m_fly = false;

	setPanAngle(panAngle);
	setTiltAngle(tiltAngle);
	setMinTiltAngle(minTiltAngle);
	setMaxTiltAngle(maxTiltAngle);
	setSteps(steps);
	setWrapPanAngle(wrapPanAngle);

	//values passed in constructor are applied immediately
	m_currentPanAngle = m_panAngle;
	m_currentTiltAngle = m_tiltAngle;
}

void FirstPersonController::setSteps(int value)
{
	if (value < 1)
		value = 1;

	if (value != m_steps)
	{
		m_steps = value;
		notifyUpdate();
	}
}

void FirstPersonController::setPanAngle(float value)
{
	if (value != m_panAngle)
	{
		m_panAngle = value;
		notifyUpdate();
	}
}

void FirstPersonController::setTiltAngle(float value)
{
	value = std::max(m_minTiltAngle, std::min(m_maxTiltAngle, value));

	if (value != m_tiltAngle)
	{
		m_tiltAngle = value;
		notifyUpdate();
	}
}

void FirstPersonController::setMinTiltAngle(float value)
{
	if (value != m_minTiltAngle)
	{
		m_minTiltAngle = value;
		setTiltAngle(std::max(m_minTiltAngle, std::min(m_maxTiltAngle, m_tiltAngle)));
	}
}

void FirstPersonController::setMaxTiltAngle(float value)
{
	if (value != m_maxTiltAngle)
	{
		m_maxTiltAngle = value;
		setTiltAngle(std::max(m_minTiltAngle, std::min(m_maxTiltAngle, m_tiltAngle)));
	}
}

void FirstPersonController::setWrapPanAngle(bool value)
{
	if (value != m_wrapPanAngle)
	{
		m_wrapPanAngle = value;
		notifyUpdate();
	}
}

void FirstPersonController::update(bool interpolate)
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
			m_currentTiltAngle = m_tiltAngle;
			m_currentPanAngle = m_panAngle;
		}

		//snap coords if angle differences are close
		if ((std::abs(m_tiltAngle - m_currentTiltAngle) < 0.01f) && (std::abs(m_panAngle - m_currentPanAngle) < 0.01f))
		{
			m_currentTiltAngle = m_tiltAngle;
			m_currentPanAngle = m_panAngle;
		}
	}

	m_targetObject->setRotationX(m_currentTiltAngle);
	m_targetObject->setRotationY(m_currentPanAngle);

	if (m_walkIncrement)
	{
		if (m_fly)
			m_targetObject->moveForward(m_walkIncrement);
		else
		{
			float panAngleRadian = m_panAngle * MathConsts::DEGREES_TO_RADIANS;
			m_targetObject->setX(m_targetObject->getX() + m_walkIncrement * std::sin(panAngleRadian));
			m_targetObject->setZ(m_targetObject->getZ() + m_walkIncrement * std::cos(panAngleRadian));
		}
		m_walkIncrement = 0;
	}

	if (m_strafeIncrement)
	{
		m_targetObject->moveRight(m_strafeIncrement);
		m_strafeIncrement = 0;
	}
}

void FirstPersonController::incrementWalk(float value)
{
	if (value != 0)
	{
		m_walkIncrement += value;
		notifyUpdate();
	}
}

void FirstPersonController::incrementStrafe(float value)
{
	if (value != 0)
	{
		m_strafeIncrement += value;
		notifyUpdate();
	}
}