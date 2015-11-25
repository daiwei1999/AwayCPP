#include "CompositeDiffuseMethod.h"
#include "ShadingMethodEvent.h"

USING_AWAY_NAMESPACE

CompositeDiffuseMethod::CompositeDiffuseMethod(ModulateFunc modulateMethod, BasicDiffuseMethod* baseDiffuseMethod)
{
	m_baseMethod = baseDiffuseMethod ? baseDiffuseMethod : new BasicDiffuseMethod();
	m_baseMethod->m_modulateMethod = modulateMethod;
	m_baseMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
}

void CompositeDiffuseMethod::setBaseMethod(BasicDiffuseMethod* value)
{
	if (m_baseMethod != value)
	{
		m_baseMethod->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		m_baseMethod = value;
		m_baseMethod->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
		invalidateShaderProgram();
	}
}

void CompositeDiffuseMethod::setShadowRegister(unsigned int value)
{
	BasicDiffuseMethod::setShadowRegister(value);
	m_baseMethod->setShadowRegister(value);
}

void CompositeDiffuseMethod::setSharedRegisters(ShaderRegisterData* value)
{
	BasicDiffuseMethod::setSharedRegisters(value);
	m_baseMethod->setSharedRegisters(value);
}

void CompositeDiffuseMethod::cleanCompilationData()
{
	BasicDiffuseMethod::cleanCompilationData();
	m_baseMethod->cleanCompilationData();
}

void CompositeDiffuseMethod::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	return m_baseMethod->getVertexCode(code, vo, regCache);
}

void CompositeDiffuseMethod::activate(MethodVO* vo, IContext* context)
{
	m_baseMethod->activate(vo, context);
}

void CompositeDiffuseMethod::deactivate(MethodVO* vo, IContext* context)
{
	m_baseMethod->deactivate(vo, context);
}

void CompositeDiffuseMethod::getFragmentPreLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	return m_baseMethod->getFragmentPreLightingCode(code, vo, regCache);
}

void CompositeDiffuseMethod::getFragmentCodePerLight(ShaderChunk& code, MethodVO* vo, unsigned int lightDirReg, unsigned int lightColReg, ShaderRegisterCache* regCache)
{
	m_baseMethod->getFragmentCodePerLight(code, vo, lightDirReg, lightColReg, regCache);
	m_totalLightColorReg = m_baseMethod->m_totalLightColorReg;
}

void CompositeDiffuseMethod::getFragmentCodePerProbe(ShaderChunk& code, MethodVO* vo, unsigned int cubeMapReg, unsigned int weightRegister, ShaderRegisterCache* regCache)
{
	m_baseMethod->getFragmentCodePerProbe(code, vo, cubeMapReg, weightRegister, regCache);
	m_totalLightColorReg = m_baseMethod->m_totalLightColorReg;
}

void CompositeDiffuseMethod::getFragmentPostLightingCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	m_baseMethod->getFragmentPostLightingCode(code, vo, regCache, targetReg);
}

void CompositeDiffuseMethod::onShaderInvalidated(Event* event)
{
	invalidateShaderProgram();
}