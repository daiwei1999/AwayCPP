#pragma once

#include "Common.h"
#include "ShadingMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * BasicNormalMethod is the default method for standard tangent-space normal mapping.
 */
class BasicNormalMethod : public ShadingMethodBase
{
public:
	BasicNormalMethod();

	void initVO(MethodVO* vo) override;
	bool isTangentSpace() { return true; }
	bool hasOutput() { return m_useTexture; }
	void copyFrom(ShadingMethodBase* method) override;

	Texture2DBase* getNormalMap() { return m_texture; }
	void setNormalMap(Texture2DBase* value);

	void cleanCompilationData() override;
	void activate(MethodVO* vo, IContext* context) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg);

protected:
	unsigned int m_normalTextureRegister;

private:
	Texture2DBase* m_texture;
	bool m_useTexture;
};

AWAY_NAMESPACE_END