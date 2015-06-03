#include "MethodVOSet.h"
#include "EffectMethodBase.h"

USING_AWAY_NAMESPACE

MethodVOSet::MethodVOSet(EffectMethodBase* method)
{
	m_method = method;
	m_data = method->createMethodVO();
}