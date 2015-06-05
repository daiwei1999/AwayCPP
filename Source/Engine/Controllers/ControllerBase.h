#pragma once 

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ControllerBase
{
public:
	ControllerBase(Entity* targetObject);

	Entity* getTargetObject() { return m_targetObject; }
	void setTargetObject(Entity* value);
	bool getAutoUpdate() { return m_autoUpdate; }
	void setAutoUpdate(bool value);

	virtual void update(bool interpolate = true) = 0;

protected:
	void notifyUpdate();

protected:
	bool m_autoUpdate;
	Entity* m_targetObject;
};

AWAY_NAMESPACE_END