#pragma once

#include "Common.h"
#include "FilterTaskBase.h"

AWAY_NAMESPACE_BEGIN

class NullFilterTask : public FilterTaskBase
{
public:
	NullFilterTask();

protected:
	void getFragmentCode(Shader& fragmentShader) override;
};

AWAY_NAMESPACE_END