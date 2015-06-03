#pragma once

#include "Common.h"
#include "ControllerBase.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * Extended camera used to automatically look at a specified target object.
 */
class LookAtController : public ControllerBase
{
public:
	LookAtController(Entity* targetObject = nullptr);

	Vector3D& getUpAxis() { return m_upAxis; }
	void setUpAxis(Vector3D& value);
	Vector3D& getLookAtPosition() { return m_lookAtPosition; }
	void setLookAtPosition(Vector3D& value);

	void update(bool interpolate = true) override;

private:
	void onLookAtObjectChanged(Event* event);

protected:
	Vector3D m_lookAtPosition;
	Vector3D m_upAxis;
};

AWAY_NAMESPACE_END