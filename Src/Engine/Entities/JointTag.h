#pragma once

#include "Common.h"
#include "ObjectContainer3D.h"

AWAY_NAMESPACE_BEGIN

class JointTag : public ObjectContainer3D
{
public:
	JointTag(int jointIndex);

protected:
	void updateSceneTransform() override;

private:
	int m_jointIndex;
};

AWAY_NAMESPACE_END