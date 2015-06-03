#include "LookAtController.h"
#include "Entity.h"

USING_AWAY_NAMESPACE

LookAtController::LookAtController(Entity* targetObject) : ControllerBase(targetObject), m_upAxis(0, 1, 0)
{
	Vector3D pos;
	setLookAtPosition(pos);
}

void LookAtController::setUpAxis(Vector3D& value)
{
	m_upAxis.copyFrom(value);
	notifyUpdate();
}

void LookAtController::setLookAtPosition(Vector3D& value)
{
	m_lookAtPosition.copyFrom(value);
	notifyUpdate();
}

void LookAtController::update(bool interpolate)
{
	if (m_targetObject)
		m_targetObject->lookAt(m_lookAtPosition, m_upAxis);
}