#include "ShaderCompiler.h"
#include "ShaderMethodSetup.h"
#include "BasicNormalMethod.h"
#include "BasicAmbientMethod.h"
#include "ShadowMapMethodBase.h"
#include "BasicDiffuseMethod.h"
#include "BasicSpecularMethod.h"
#include "MethodVO.h"
#include "Regs.h"
#include "MethodVOSet.h"
#include "EffectMethodBase.h"
#include "ColorTransformMethod.h"
#include "CompiledPass.h"

USING_AWAY_NAMESPACE

ShaderCompiler::ShaderCompiler()
{
	m_registerCache.setVertexAttributesOffset(1);
	m_registerCache.reset();

	m_commonsDataIndex = -1;
	m_uvBufferIndex = -1;
	m_uvTransformIndex = -1;
	m_secondaryUVBufferIndex = -1;
	m_normalBufferIndex = -1;
	m_tangentBufferIndex = -1;
	m_lightFragmentConstantIndex = -1;
	m_sceneMatrixIndex = -1;
	m_sceneNormalMatrixIndex = -1;
	m_cameraPositionIndex = -1;
	m_probeWeightsIndex = -1;
	m_needUVAnimation = false;
}

ShaderCompiler::~ShaderCompiler()
{

}

void ShaderCompiler::setTextureSampling(bool smooth, bool repeat, bool mipmap)
{
	m_smooth = smooth;
	m_repeat = repeat;
	m_mipmap = mipmap;
}

void ShaderCompiler::setConstantDataBuffers(std::vector<float>* vertexConstantData, std::vector<float>* fragmentConstantData)
{
	m_vertexConstantData = vertexConstantData;
	m_fragmentConstantData = fragmentConstantData;
}

bool ShaderCompiler::usesNormals()
{
	return m_dependencyCounter.getNormalDependencies() > 0 && m_methodSetup->m_normalMethod->hasOutput();
}

bool ShaderCompiler::usesLights()
{
	return m_numLights > 0 && (m_combinedLightSources & static_cast<unsigned int>(LightSources::LIGHTS)) != 0;
}

void ShaderCompiler::compile()
{
	initRegisterIndices();
	initLightData();

	m_animatableAttributes.push_back(Regs::a0);
	m_animationTargetRegisters.push_back(Regs::t0);

	m_sharedRegisters.m_localPosition = m_registerCache.getFreeVertexVectorTemp();
	m_registerCache.addVertexTempUsages(m_sharedRegisters.m_localPosition, 1);

	createCommons();
	calculateDependencies();
	updateMethodRegisters();

	for (int i = 0; i < 4; i++)
		m_registerCache.getFreeVertexConstant();

	createNormalRegisters();

	if (m_dependencyCounter.getGlobalPosDependencies() > 0 || m_forceSeperateMVP)
		compileGlobalPositionCode();

	compileProjectionCode();
	compileMethodsCode();
	compileFragmentOutput();
}

void ShaderCompiler::createNormalRegisters()
{

}

void ShaderCompiler::compileLightingCode()
{

}

void ShaderCompiler::compileViewDirCode()
{

}

void ShaderCompiler::compileNormalCode()
{

}

void ShaderCompiler::initRegisterIndices()
{
	m_commonsDataIndex = -1;
	m_cameraPositionIndex = -1;
	m_uvBufferIndex = -1;
	m_uvTransformIndex = -1;
	m_secondaryUVBufferIndex = -1;
	m_normalBufferIndex = -1;
	m_tangentBufferIndex = -1;
	m_lightFragmentConstantIndex = -1;
	m_sceneMatrixIndex = -1;
	m_sceneNormalMatrixIndex = -1;
	m_probeWeightsIndex = -1;
}

void ShaderCompiler::initLightData()
{
	m_numLights = m_numPointLights + m_numDirectionalLights;
	m_numProbeRegisters = (int)std::ceil(m_numLightProbes / 4.f);

	if (m_methodSetup->m_specularMethod)
		m_combinedLightSources = m_specularLightSources | m_diffuseLightSources;
	else
		m_combinedLightSources = m_diffuseLightSources;

	m_usingSpecularMethod = m_methodSetup->m_specularMethod && (usesLightsForSpecular() || usesProbesForSpecular());
}

void ShaderCompiler::calculateDependencies()
{
	m_dependencyCounter.reset();
	setupAndCountMethodDependencies(m_methodSetup->m_diffuseMethod, m_methodSetup->m_diffuseMethodVO);

	if (m_methodSetup->m_shadowMethod)
		setupAndCountMethodDependencies(m_methodSetup->m_shadowMethod, m_methodSetup->m_shadowMethodVO);

	setupAndCountMethodDependencies(m_methodSetup->m_ambientMethod, m_methodSetup->m_ambientMethodVO);

	if (m_usingSpecularMethod)
		setupAndCountMethodDependencies(m_methodSetup->m_specularMethod, m_methodSetup->m_specularMethodVO);

	if (m_methodSetup->m_colorTransformMethod)
		setupAndCountMethodDependencies(m_methodSetup->m_colorTransformMethod, m_methodSetup->m_colorTransformMethodVO);

	for (auto set : m_methodSetup->m_methods)
		setupAndCountMethodDependencies(set->m_method, set->m_data);

	if (usesNormals())
		setupAndCountMethodDependencies(m_methodSetup->m_normalMethod, m_methodSetup->m_normalMethodVO);

	// todo: add spotlights to count check
	m_dependencyCounter.setPositionedLights(m_numPointLights, m_combinedLightSources);
}

bool ShaderCompiler::usesLightsForSpecular()
{
	return m_numLights > 0 && (m_specularLightSources & static_cast<unsigned int>(LightSources::LIGHTS)) != 0;
}

bool ShaderCompiler::usesLightsForDiffuse()
{
	return m_numLights > 0 && (m_diffuseLightSources & static_cast<unsigned int>(LightSources::LIGHTS)) != 0;
}

bool ShaderCompiler::usesProbesForSpecular()
{
	return m_numLightProbes > 0 && (m_specularLightSources & static_cast<unsigned int>(LightSources::PROBES)) != 0;
}

bool ShaderCompiler::usesProbesForDiffuse()
{
	return m_numLightProbes > 0 && (m_diffuseLightSources & static_cast<unsigned int>(LightSources::PROBES)) != 0;
}

bool ShaderCompiler::usesProbes()
{
	return m_numLightProbes > 0 && ((m_diffuseLightSources | m_specularLightSources) & static_cast<unsigned int>(LightSources::PROBES)) != 0;
}

void ShaderCompiler::compileMethods()
{
	for (auto set : m_methodSetup->m_methods)
	{
		EffectMethodBase* method = set->m_method;
		MethodVO* data = set->m_data;

		method->getVertexCode(m_vertexCode, data, &m_registerCache);

		if (data->m_needsGlobalVertexPos || data->m_needsGlobalFragmentPos)
			m_registerCache.removeVertexTempUsage(m_sharedRegisters.m_globalPositionVertex);

		method->getFragmentCode(m_fragmentPostLightCode, data, &m_registerCache, m_sharedRegisters.m_shadedTarget);

		if (data->m_needsNormals)
			m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_normalFragment);

		if (data->m_needsView)
			m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_viewDirFragment);
	}

	if (m_methodSetup->m_colorTransformMethod)
	{
		m_methodSetup->m_colorTransformMethod->getVertexCode(m_vertexCode, m_methodSetup->m_colorTransformMethodVO, &m_registerCache);
		m_methodSetup->m_colorTransformMethod->getFragmentCode(m_fragmentPostLightCode, m_methodSetup->m_colorTransformMethodVO, &m_registerCache, m_sharedRegisters.m_shadedTarget);
	}
}

void ShaderCompiler::compileMethodsCode()
{
	if (m_dependencyCounter.getUVDependencies() > 0)
		compileUVCode();

	if (m_dependencyCounter.getSecondaryUVDependencies() > 0)
		compileSecondaryUVCode();

	if (m_dependencyCounter.getNormalDependencies() > 0)
		compileNormalCode();

	if (m_dependencyCounter.getViewDirDependencies() > 0)
		compileViewDirCode();

	compileLightingCode();
	compileMethods();
}

void ShaderCompiler::compileUVCode()
{
	unsigned int uvAttributeReg = m_registerCache.getFreeVertexAttribute();
	m_sharedRegisters.m_uvInput = uvAttributeReg;
	m_uvBufferIndex = REGISTER_INDEX(uvAttributeReg);

	unsigned int varying = m_registerCache.getFreeVarying();
	m_sharedRegisters.m_uvVarying = varying;

	if (m_animateUVs)
	{
		unsigned int uvTransform1 = m_registerCache.getFreeVertexConstant();
		unsigned int uvTransform2 = m_registerCache.getFreeVertexConstant();
		m_uvTransformIndex = REGISTER_INDEX(uvTransform1) * 4;

		m_vertexCode.dp4(varying ^ Regs::x, uvAttributeReg, uvTransform1);
		m_vertexCode.dp4(varying ^ Regs::y, uvAttributeReg, uvTransform2);
		m_vertexCode.mov(varying ^ Regs::zw, uvAttributeReg ^ Regs::zw);
	}
	else
	{
		m_uvTransformIndex = -1;
		m_needUVAnimation = true;
		m_uvTarget = varying;
		m_uvSource = uvAttributeReg;
	}
}

void ShaderCompiler::compileSecondaryUVCode()
{
	m_sharedRegisters.m_secondaryUVInput = m_registerCache.getFreeVertexAttribute();
	m_secondaryUVBufferIndex = REGISTER_INDEX(m_sharedRegisters.m_secondaryUVInput);
	m_sharedRegisters.m_secondaryUVVarying = m_registerCache.getFreeVarying();
	m_vertexCode.mov(m_sharedRegisters.m_secondaryUVVarying, m_sharedRegisters.m_secondaryUVInput);
}

void ShaderCompiler::compileGlobalPositionCode()
{
	m_sharedRegisters.m_globalPositionVertex = m_registerCache.getFreeVertexVectorTemp();
	m_registerCache.addVertexTempUsages(m_sharedRegisters.m_globalPositionVertex, m_dependencyCounter.getGlobalPosDependencies());

	unsigned int positionMatrixReg = m_registerCache.getFreeVertexConstant();
	m_registerCache.getFreeVertexConstant();
	m_registerCache.getFreeVertexConstant();
	m_registerCache.getFreeVertexConstant();
	m_sceneMatrixIndex = REGISTER_INDEX(positionMatrixReg) * 4;

	m_vertexCode.m44(m_sharedRegisters.m_globalPositionVertex, m_sharedRegisters.m_localPosition, positionMatrixReg);

	if (m_dependencyCounter.usesGlobalPosFragment())
	{
		m_sharedRegisters.m_globalPositionVarying = m_registerCache.getFreeVarying();
		m_vertexCode.mov(m_sharedRegisters.m_globalPositionVarying, m_sharedRegisters.m_globalPositionVertex);
	}
}

void ShaderCompiler::compileProjectionCode()
{
	unsigned int pos = (m_dependencyCounter.getGlobalPosDependencies() > 0 || m_forceSeperateMVP) ? m_sharedRegisters.m_globalPositionVertex : m_animationTargetRegisters[0];
	if (m_dependencyCounter.getProjectionDependencies() > 0)
	{
		m_sharedRegisters.m_projectivePositionVarying = m_registerCache.getFreeVarying();
		m_vertexCode.m44(Regs::t5, pos, Regs::c0);
		m_vertexCode.mov(m_sharedRegisters.m_projectivePositionVarying, Regs::t5);
		m_vertexCode.mov(Regs::op, Regs::t5);
	}
	else
		m_vertexCode.m44(Regs::op, pos, Regs::c0);
}

void ShaderCompiler::compileFragmentOutput()
{
	m_fragmentPostLightCode.mov(Regs::oc, m_sharedRegisters.m_shadedTarget);
	m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_shadedTarget);
}

void ShaderCompiler::createCommons()
{
	m_sharedRegisters.m_commons = m_registerCache.getFreeFragmentConstant();
	m_commonsDataIndex = REGISTER_INDEX(m_sharedRegisters.m_commons) * 4;
}

void ShaderCompiler::setupAndCountMethodDependencies(ShadingMethodBase* method, MethodVO* methodVO)
{
	setupMethod(method, methodVO);
	m_dependencyCounter.includeMethodVO(methodVO);
}

void ShaderCompiler::setupMethod(ShadingMethodBase* method, MethodVO* methodVO)
{
	method->reset();
	methodVO->reset();
	methodVO->m_vertexData = m_vertexConstantData;
	methodVO->m_fragmentData = m_fragmentConstantData;
	methodVO->m_useSmoothTextures = m_smooth;
	methodVO->m_repeatTextures = m_repeat;
	methodVO->m_useMipmapping = m_mipmap;
	methodVO->m_useLightFallOff = m_enableLightFallOff;
	methodVO->m_numLights = m_numLights + m_numLightProbes;
	method->initVO(methodVO);
}

void ShaderCompiler::updateMethodRegisters()
{
	m_methodSetup->m_normalMethod->setSharedRegisters(&m_sharedRegisters);
	m_methodSetup->m_diffuseMethod->setSharedRegisters(&m_sharedRegisters);

	if (m_methodSetup->m_shadowMethod)
		m_methodSetup->m_shadowMethod->setSharedRegisters(&m_sharedRegisters);

	m_methodSetup->m_ambientMethod->setSharedRegisters(&m_sharedRegisters);

	if (m_methodSetup->m_specularMethod)
		m_methodSetup->m_specularMethod->setSharedRegisters(&m_sharedRegisters);

	if (m_methodSetup->m_colorTransformMethod)
		m_methodSetup->m_colorTransformMethod->setSharedRegisters(&m_sharedRegisters);

	for (auto set : m_methodSetup->m_methods)
		set->m_method->setSharedRegisters(&m_sharedRegisters);
}

void ShaderCompiler::cleanUpMethods()
{
	if (m_methodSetup->m_normalMethod)
		m_methodSetup->m_normalMethod->cleanCompilationData();

	if (m_methodSetup->m_diffuseMethod)
		m_methodSetup->m_diffuseMethod->cleanCompilationData();

	if (m_methodSetup->m_ambientMethod)
		m_methodSetup->m_ambientMethod->cleanCompilationData();

	if (m_methodSetup->m_specularMethod)
		m_methodSetup->m_specularMethod->cleanCompilationData();

	if (m_methodSetup->m_shadowMethod)
		m_methodSetup->m_shadowMethod->cleanCompilationData();

	if (m_methodSetup->m_colorTransformMethod)
		m_methodSetup->m_colorTransformMethod->cleanCompilationData();

	for (auto set : m_methodSetup->m_methods)
		set->m_method->cleanCompilationData();
}