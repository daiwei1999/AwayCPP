#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

class LightMapMethod : public EffectMethodBase
{
public:
	LightMapMethod(Texture2DBase* texture);

	Texture2DBase* getTexture() { return m_texture; }
	void setTexture(Texture2DBase* value) { m_texture = value; }
	void setUVTransform(float scaleU, float scaleV, float offsetU, float offsetV);
	void setColorScale(float scaleR, float scaleG, float scaleB);

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void activate(MethodVO* vo, IContext* context) override;
	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	Texture2DBase* m_texture;
	float m_scaleU;
	float m_scaleV;
	float m_offsetU;
	float m_offsetV;
	float m_scaleR;
	float m_scaleG;
	float m_scaleB;
};

AWAY_NAMESPACE_END