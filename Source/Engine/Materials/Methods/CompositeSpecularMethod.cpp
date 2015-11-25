#include "CompositeSpecularMethod.h"
#include "ShadingMethodEvent.h"

USING_AWAY_NAMESPACE

CompositeSpecularMethod::CompositeSpecularMethod(ModulateFunc modulateMethod, BasicSpecularMethod* baseSpecularMethod)
{
	m_baseMethod = baseSpecularMethod ? baseSpecularMethod : new BasicSpecularMethod();
	m_baseMethod->m_modulateMethod = modulateMethod;
	m_baseMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
}

void CompositeSpecularMethod::setBaseMethod(BasicSpecularMethod* value)
{
	if (m_baseMethod != value)
	{
		m_baseMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_baseMethod = value;
		m_baseMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		invalidateShaderProgram();
	}
}

void CompositeSpecularMethod::setShadowRegister(unsigned int value)
{
	BasicSpecularMethod::setShadowRegister(value);
	m_baseMethod->setShadowRegister(value);
}

void CompositeSpecularMethod::setSharedRegisters(ShaderRegisterData* value)
{
	BasicSpecularMethod::setSharedRegisters(value);
	m_baseMethod->setSharedRegisters(value);
}

void CompositeSpecularMethod::cleanCompilationData()
{
	BasicSpecularMethod::cleanCompilationData();
	m_baseMethod->cleanCompilationData();
}

void CompositeSpecularMethod::onShaderInvalidated(Event* event)
{
	invalidateShaderProgram();
}