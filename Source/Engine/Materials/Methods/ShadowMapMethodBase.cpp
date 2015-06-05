#include "ShadowMapMethodBase.h"
#include "LightBase.h"

USING_AWAY_NAMESPACE

ShadowMapMethodBase::ShadowMapMethodBase(LightBase* castingLight)
{
	m_castingLight = castingLight;
	m_castingLight->setCastsShadows(true);
	m_shadowMapper = m_castingLight->getShadowMapper();
	m_epsilon = 0.1f;
	m_alpha = 1;
}