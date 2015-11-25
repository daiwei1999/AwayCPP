#pragma once

#include "Common.h"
#include "BasicDiffuseMethod.h"

AWAY_NAMESPACE_BEGIN

/**
 * CompositeDiffuseMethod provides a base class for diffuse methods that wrap a diffuse method to alter the
 * calculated diffuse reflection strength.
 */
class CompositeDiffuseMethod : public BasicDiffuseMethod
{
	DEFINE_CALLBACK_CLASS(CompositeDiffuseMethod, onShaderInvalidated)

public:
	CompositeDiffuseMethod(ModulateFunc modulateMethod, BasicDiffuseMethod* baseDiffuseMethod = nullptr);

	BasicDiffuseMethod* getBaseMethod() { return m_baseMethod; }
	void setBaseMethod(BasicDiffuseMethod* value);
	float getAlphaThreshold() override { return m_baseMethod->getAlphaThreshold(); }
	void setAlphaThreshold(float value) override { m_baseMethod->setAlphaThreshold(value); }
	Texture2DBase* getTexture() override { return m_baseMethod->getTexture(); }
	void setTexture(Texture2DBase* value) override { m_baseMethod->setTexture(value); }
	float getDiffuseAlpha() override { return m_baseMethod->getDiffuseAlpha(); }
	void setDiffuseAlpha(float value) override { m_baseMethod->setDiffuseAlpha(value); }
	unsigned int getDiffuseColor() override { return m_baseMethod->getDiffuseColor(); }
	void setDiffuseColor(unsigned int value) override { m_baseMethod->setDiffuseColor(value); }
	void setShadowRegister(unsigned int value) override;
	void setSharedRegisters(ShaderRegisterData* value) override;

	void initVO(MethodVO* vo) override { m_baseMethod->initVO(vo); }
	void initConstants(MethodVO* vo) override { m_baseMethod->initConstants(vo); }
	void reset() override { m_baseMethod->reset(); }
	void cleanCompilationData() override;
	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void activate(MethodVO* vo, IContext* context) override;
	void deactivate(MethodVO* vo, IContext* context) override;
	void getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache) override;
	void getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache) override;
	void getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	
private:
	void onShaderInvalidated(Event* event);

protected:
	BasicDiffuseMethod* m_baseMethod;
};

AWAY_NAMESPACE_END