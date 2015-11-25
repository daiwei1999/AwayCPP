#pragma once

#include "Common.h"
#include "BasicSpecularMethod.h"

AWAY_NAMESPACE_BEGIN

/**
 * CompositeSpecularMethod provides a base class for specular methods that wrap a specular method to alter the
 * calculated specular reflection strength.
 */
class CompositeSpecularMethod : public BasicSpecularMethod
{
	DEFINE_CALLBACK_CLASS(CompositeSpecularMethod, onShaderInvalidated)

public:
	CompositeSpecularMethod(ModulateFunc modulateMethod, BasicSpecularMethod* baseSpecularMethod = nullptr);

	BasicSpecularMethod* getBaseMethod() { return m_baseMethod; }
	void setBaseMethod(BasicSpecularMethod* value);
	float getGloss() override { return m_baseMethod->getGloss(); }
	void setGloss(float value) override { m_baseMethod->setGloss(value); }
	float getSpecular() override { return m_baseMethod->getSpecular(); }
	void setSpecular(float value) override { m_baseMethod->setSpecular(value); }
	Texture2DBase* getTexture() override { return m_baseMethod->getTexture(); }
	void setTexture(Texture2DBase* value) override { m_baseMethod->setTexture(value); }

	MaterialPassVector& getPasses() override { return m_baseMethod->getPasses(); }
	void setShadowRegister(unsigned int value) override;
	void setSharedRegisters(ShaderRegisterData* value) override;

	void initVO(MethodVO* vo) override { m_baseMethod->initVO(vo); }
	void initConstants(MethodVO* vo) override { m_baseMethod->initConstants(vo); }
	void reset() override { m_baseMethod->reset(); }
	void cleanCompilationData() override;
	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override { m_baseMethod->getVertexCode(code, vo, regCache); }
	void activate(MethodVO* vo, IContext* context) override { m_baseMethod->activate(vo, context); }
	void deactivate(MethodVO* vo, IContext* context) override { m_baseMethod->deactivate(vo, context); }
	void getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override { m_baseMethod->getFragmentPreLightingCode(code, vo, regCache); }
	void getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache) override { m_baseMethod->getFragmentCodePerLight(code, vo, lightDirReg, lightColReg, regCache); }
	void getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache) override { m_baseMethod->getFragmentCodePerProbe(code, vo, cubeMapReg, weightRegister, regCache); }
	void getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override { m_baseMethod->getFragmentPostLightingCode(code, vo, regCache, targetReg); }

private:
	void onShaderInvalidated(Event* event);

private:
	BasicSpecularMethod* m_baseMethod;
};

AWAY_NAMESPACE_END