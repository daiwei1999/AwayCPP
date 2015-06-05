#pragma once

#include "Common.h"
#include "Entity.h"

AWAY_NAMESPACE_BEGIN

enum class LightType
{
	Point,       // Point light
	Directional, // Directional light
	Probe        // Light probe
};

/**
 * LightBase provides an abstract base class for subtypes representing lights.
 */
class LightBase : public Entity
{
public:
	LightBase();

	bool getCastsShadows() { return m_castsShadows; }
	void setCastsShadows(bool value);
	float getSpecular() { return m_specular; }
	void setSpecular(float value);
	float getDiffuse() { return m_diffuse; }
	void setDiffuse(float value);
	unsigned int getColor() { return m_color; }
	void setColor(unsigned int value);
	float getAmbient() { return m_ambient; }
	void setAmbient(float value);
	unsigned int getAmbientColor() { return m_ambientColor; }
	void setAmbientColor(unsigned int value);
	ShadowMapperBase* getShadowMapper() { return m_shadowMapper; }
	void setShadowMapper(ShadowMapperBase* value);

	LightType getType() { return m_type; }
	float getAmbientR() { return m_ambientR; }
	float getAmbientG() { return m_ambientG; }
	float getAmbientB() { return m_ambientB; }
	float getDiffuseR() { return m_diffuseR; }
	float getDiffuseG() { return m_diffuseG; }
	float getDiffuseB() { return m_diffuseB; }
	float getSpecularR() { return m_specularR; }
	float getSpecularG() { return m_specularG; }
	float getSpecularB() { return m_specularB; }

	virtual void getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result) = 0;

protected:
	EntityNode* createEntityPartitionNode() override;
	virtual ShadowMapperBase* createShadowMapper() = 0;

private:
	void updateAmbient();
	void updateSpecular();
	void updateDiffuse();

protected:
	LightType m_type;

private:
	unsigned int m_color;
	float m_colorR, m_colorG, m_colorB;
	unsigned int m_ambientColor;
	float m_ambient;
	float m_ambientR, m_ambientG, m_ambientB;
	float m_specular;
	float m_specularR, m_specularG, m_specularB;
	float m_diffuse;
	float m_diffuseR, m_diffuseG, m_diffuseB;
	bool m_castsShadows;
	ShadowMapperBase* m_shadowMapper;
};

AWAY_NAMESPACE_END