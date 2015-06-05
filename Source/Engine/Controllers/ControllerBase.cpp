#include "ControllerBase.h"
#include "Entity.h"
#include "Partition3D.h"

USING_AWAY_NAMESPACE

ControllerBase::ControllerBase(Entity* targetObject)
{
	m_autoUpdate = true;
	m_targetObject = nullptr;
	setTargetObject(targetObject);
}

void ControllerBase::setTargetObject(Entity* value)
{
	if (value == m_targetObject)
		return;

	if (m_targetObject && m_autoUpdate)
		m_targetObject->setController(nullptr);

	m_targetObject = value;

	if (m_targetObject && m_autoUpdate)
		m_targetObject->setController(this);

	notifyUpdate();
}

void ControllerBase::setAutoUpdate(bool value)
{
	if (value == m_autoUpdate)
		return;

	m_autoUpdate = value;

	if (m_targetObject)
	{
		if (m_autoUpdate)
			m_targetObject->setController(this);
		else
			m_targetObject->setController(nullptr);
	}
}

void ControllerBase::notifyUpdate()
{
	if (m_targetObject && m_targetObject->getImplicitPartition() && m_autoUpdate)
		m_targetObject->getImplicitPartition()->markForUpdate(m_targetObject);
}