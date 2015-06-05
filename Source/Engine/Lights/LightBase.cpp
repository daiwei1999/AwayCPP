#include "LightBase.h"
#include "ShadowMapperBase.h"
#include "LightNode.h"
#include "LightEvent.h"

USING_AWAY_NAMESPACE

LightBase::LightBase()
{
	m_color = 0xffffff;
	m_colorR = m_colorG = m_colorB = 1;
	m_ambientColor = 0xffffff;
	m_ambient = 0;
	m_ambientR = m_ambientG = m_ambientB = 0;
	m_specular = 1;
	m_specularR = m_specularG = m_specularB = 1;
	m_diffuse = 1;
	m_diffuseR = m_diffuseG = m_diffuseB = 1;
	m_castsShadows = false;
	m_shadowMapper = nullptr;
}

void LightBase::setCastsShadows(bool value)
{
	if (m_castsShadows == value)
		return;

	m_castsShadows = value;
	if (value)
	{
		if (!m_shadowMapper)
			m_shadowMapper = createShadowMapper();

		m_shadowMapper->setLight(this);
	}
	else
	{
		delete m_shadowMapper;
		m_shadowMapper = nullptr;
	}

	LightEvent event(LightEvent::CASTS_SHADOW_CHANGE, this);
	dispatchEvent(&event);
}

void LightBase::setSpecular(float value)
{
	if (value < 0)
		value = 0;

	m_specular = value;
	updateSpecular();
}

void LightBase::setDiffuse(float value)
{
	if (value < 0)
		value = 0;

	m_diffuse = value;
	updateDiffuse();
}

void LightBase::setColor(unsigned int value)
{
	m_color = value;
	m_colorR = ((m_color >> 16) & 0xff) / 255.f;
	m_colorG = ((m_color >> 8) & 0xff) / 255.f;
	m_colorB = (m_color & 0xff) / 255.f;
	updateDiffuse();
	updateSpecular();
}

void LightBase::setAmbient(float value)
{
	if (value < 0)
		value = 0;
	else if (value > 1)
		value = 1;

	m_ambient = value;
	updateAmbient();
}

void LightBase::setAmbientColor(unsigned int value)
{
	m_ambientColor = value;
	updateAmbient();
}

void LightBase::setShadowMapper(ShadowMapperBase* value)
{
	m_shadowMapper = value;
	m_shadowMapper->setLight(this);
}

EntityNode* LightBase::createEntityPartitionNode()
{
	return new LightNode(this);
}

void LightBase::updateAmbient()
{
	m_ambientR = ((m_ambientColor >> 16) & 0xff) / 255.f * m_ambient;
	m_ambientG = ((m_ambientColor >> 8) & 0xff) / 255.f * m_ambient;
	m_ambientB = (m_ambientColor & 0xff) / 255.f * m_ambient;
}

void LightBase::updateSpecular()
{
	m_specularR = m_colorR * m_specular;
	m_specularG = m_colorG * m_specular;
	m_specularB = m_colorB * m_specular;
}

void LightBase::updateDiffuse()
{
	m_diffuseR = m_colorR * m_diffuse;
	m_diffuseG = m_colorG * m_diffuse;
	m_diffuseB = m_colorB * m_diffuse;
}