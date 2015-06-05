#pragma once

#include "Common.h"
#include "EventDispatcher.h"

AWAY_NAMESPACE_BEGIN

/**
 * ShaderMethodSetup contains the method configuration for an entire material.
 */
class ShaderMethodSetup : public EventDispatcher
{
	DEFINE_CALLBACK_CLASS(ShaderMethodSetup, onShaderInvalidated)

public:
	ShaderMethodSetup();

	ColorTransformMethod* getColorTransformMethod() { return m_colorTransformMethod; }
	void setColorTransformMethod(ColorTransformMethod* value);
	BasicNormalMethod* getNormalMethod() { return m_normalMethod; }
	void setNormalMethod(BasicNormalMethod* value);
	BasicAmbientMethod* getAmbientMethod() { return m_ambientMethod; }
	void setAmbientMethod(BasicAmbientMethod* value);
	ShadowMapMethodBase* getShadowMethod() { return m_shadowMethod; }
	void setShadowMethod(ShadowMapMethodBase* value);
	BasicDiffuseMethod* getDiffuseMethod() { return m_diffuseMethod; }
	void setDiffuseMethod(BasicDiffuseMethod* value);
	BasicSpecularMethod* getSpecularMethod() { return m_specularMethod; }
	void setSpecularMethod(BasicSpecularMethod* value);

	void addMethod(EffectMethodBase* method);
	void addMethodAt(EffectMethodBase* method, int index);
	void removeMethod(EffectMethodBase* method);
	void removeAllMethod();
	bool hasMethod(EffectMethodBase* method);
	EffectMethodBase* getMethodAt(unsigned int index);
	int getMethodCount() { return m_methods.size(); }

private:
	void onShaderInvalidated(Event* event);
	void invalidateShaderProgram();

	friend class CompiledPass;
	friend class SuperShaderPass;
	friend class ShaderCompiler;
	friend class SuperShaderCompiler;
private:
	ColorTransformMethod* m_colorTransformMethod;
	MethodVO* m_colorTransformMethodVO;
	BasicNormalMethod* m_normalMethod;
	MethodVO* m_normalMethodVO;
	BasicAmbientMethod* m_ambientMethod;
	MethodVO* m_ambientMethodVO;
	ShadowMapMethodBase* m_shadowMethod;
	MethodVO* m_shadowMethodVO;
	BasicDiffuseMethod* m_diffuseMethod;
	MethodVO* m_diffuseMethodVO;
	BasicSpecularMethod* m_specularMethod;
	MethodVO* m_specularMethodVO;
	std::vector<MethodVOSet*> m_methods;
};

AWAY_NAMESPACE_END