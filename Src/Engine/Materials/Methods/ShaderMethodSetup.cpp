#include "ShaderMethodSetup.h"
#include "ColorTransformMethod.h"
#include "BasicNormalMethod.h"
#include "BasicAmbientMethod.h"
#include "ShadowMapMethodBase.h"
#include "BasicDiffuseMethod.h"
#include "BasicSpecularMethod.h"
#include "MethodVOSet.h"
#include "ShadingMethodEvent.h"

USING_AWAY_NAMESPACE

ShaderMethodSetup::ShaderMethodSetup()
{
	m_colorTransformMethod = nullptr;
	m_normalMethod = new BasicNormalMethod();
	m_ambientMethod = new BasicAmbientMethod();
	m_shadowMethod = nullptr;
	m_diffuseMethod = new BasicDiffuseMethod();
	m_specularMethod = new BasicSpecularMethod();

	// addEventListener to ShadingMethodEvent.SHADER_INVALIDATED
	m_normalMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	m_ambientMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	m_diffuseMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	m_specularMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	
	m_colorTransformMethodVO = nullptr;
	m_normalMethodVO = m_normalMethod->createMethodVO();
	m_ambientMethodVO = m_ambientMethod->createMethodVO();
	m_shadowMethodVO = nullptr;
	m_diffuseMethodVO = m_diffuseMethod->createMethodVO();
	m_specularMethodVO = m_specularMethod->createMethodVO();
}

void ShaderMethodSetup::setColorTransformMethod(ColorTransformMethod* value)
{
	if (value == m_colorTransformMethod)
		return;

	if (m_colorTransformMethod)
		m_colorTransformMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));

	if (!m_colorTransformMethod || !value)
		invalidateShaderProgram();

	m_colorTransformMethod = value;

	if (m_colorTransformMethod)
	{
		m_colorTransformMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_colorTransformMethodVO = m_colorTransformMethod->createMethodVO();
	}
	else
		m_colorTransformMethodVO = nullptr;
}

void ShaderMethodSetup::setNormalMethod(BasicNormalMethod* value)
{
	if (m_normalMethod)
		m_normalMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));

	if (value)
	{
		if (m_normalMethod)
			value->copyFrom(m_normalMethod);

		value->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_normalMethodVO = value->createMethodVO();
	}

	m_normalMethod = value;

	if (m_normalMethod)
		invalidateShaderProgram();
}

void ShaderMethodSetup::setAmbientMethod(BasicAmbientMethod* value)
{
	if (m_ambientMethod)
		m_ambientMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));

	if (value)
	{
		if (m_ambientMethod)
			value->copyFrom(m_ambientMethod);

		value->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_ambientMethodVO = value->createMethodVO();
	}

	m_ambientMethod = value;

	if (m_ambientMethod)
		invalidateShaderProgram();
}

void ShaderMethodSetup::setShadowMethod(ShadowMapMethodBase* value)
{
	if (m_shadowMethod)
		m_shadowMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));

	m_shadowMethod = value;

	if (m_shadowMethod)
	{
		m_shadowMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_shadowMethodVO = m_shadowMethod->createMethodVO();
	}
	else
		m_shadowMethodVO = nullptr;

	invalidateShaderProgram();
}

void ShaderMethodSetup::setDiffuseMethod(BasicDiffuseMethod* value)
{
	if (m_diffuseMethod)
		m_diffuseMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));

	if (value)
	{
		if (m_diffuseMethod)
			value->copyFrom(m_diffuseMethod);

		value->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_diffuseMethodVO = value->createMethodVO();
	}

	m_diffuseMethod = value;

	if (m_diffuseMethod)
		invalidateShaderProgram();
}

void ShaderMethodSetup::setSpecularMethod(BasicSpecularMethod* value)
{
	if (m_specularMethod)
	{
		m_specularMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		if (value)
			value->copyFrom(m_specularMethod);
	}

	m_specularMethod = value;

	if (m_specularMethod)
	{
		m_specularMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_specularMethodVO = m_specularMethod->createMethodVO();
	}
	else
		m_specularMethodVO = nullptr;

	invalidateShaderProgram();
}

void ShaderMethodSetup::addMethod(EffectMethodBase* method)
{
	m_methods.push_back(new MethodVOSet(method));
	method->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	invalidateShaderProgram();
}

void ShaderMethodSetup::removeMethod(EffectMethodBase* method)
{
	for (std::vector<MethodVOSet*>::iterator iter = m_methods.begin(); iter != m_methods.end(); iter++)
	{
		MethodVOSet* methodSet = *iter;
		if (methodSet->m_method == method)
		{
			delete methodSet;
			m_methods.erase(iter);
			method->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
			invalidateShaderProgram();
			break;
		}
	}
}

void ShaderMethodSetup::removeAllMethod()
{
	for (auto set : m_methods)
	{
		set->m_method->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		delete set;
	}

	m_methods.clear();
	invalidateShaderProgram();
}

bool ShaderMethodSetup::hasMethod(EffectMethodBase* method)
{
	for (auto set : m_methods)
	{
		if (set->m_method == method)
			return true;
	}
	return false;
}

void ShaderMethodSetup::addMethodAt(EffectMethodBase* method, int index)
{
	m_methods.insert(m_methods.begin() + index, new MethodVOSet(method));
	method->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	invalidateShaderProgram();
}

EffectMethodBase* ShaderMethodSetup::getMethodAt(unsigned int index)
{
	if (index >= m_methods.size())
		return nullptr;

	return m_methods[index]->m_method;
}

void ShaderMethodSetup::onShaderInvalidated(Event* event)
{
	invalidateShaderProgram();
}

void ShaderMethodSetup::invalidateShaderProgram()
{
	ShadingMethodEvent event(ShadingMethodEvent::SHADER_INVALIDATED);
	dispatchEvent(&event);
}