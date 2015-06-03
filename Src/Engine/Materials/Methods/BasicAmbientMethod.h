#pragma once

#include "Common.h"
#include "ShadingMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * BasicAmbientMethod provides the default shading method for uniform ambient lighting.
 */
class BasicAmbientMethod : public ShadingMethodBase
{
public:
	BasicAmbientMethod();

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void cleanCompilationData() override;
	void copyFrom(ShadingMethodBase* method) override;

	float getAmbient() { return m_ambient; }
	void setAmbient(float value) { m_ambient = value; }

	unsigned int getAmbientColor() { return m_ambientColor; }
	void setAmbientColor(unsigned int value) { m_ambientColor = value; }

	Texture2DBase* getTexture() { return m_texture; }
	void setTexture(Texture2DBase* value);

	void activate(MethodVO* vo, IContext* context) override;
	void setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg);

public:
	float m_lightAmbientR, m_lightAmbientG, m_lightAmbientB;

protected:
	bool m_useTexture;
	unsigned int m_ambientInputRegister;

private:
	Texture2DBase* m_texture;
	unsigned int m_ambientColor;
	float m_ambientR, m_ambientG, m_ambientB;
	float m_ambient;
};

AWAY_NAMESPACE_END