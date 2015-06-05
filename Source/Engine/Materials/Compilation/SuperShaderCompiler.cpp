#include "SuperShaderCompiler.h"
#include "ShaderChunk.h"
#include "ShaderMethodSetup.h"
#include "BasicNormalMethod.h"
#include "BasicAmbientMethod.h"
#include "ShadowMapMethodBase.h"
#include "BasicDiffuseMethod.h"
#include "BasicSpecularMethod.h"
#include "MethodVO.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

SuperShaderCompiler::SuperShaderCompiler()
{

}

void SuperShaderCompiler::initLightData()
{
	ShaderCompiler::initLightData();

	m_pointLightRegisters.resize(m_numPointLights * 3);
	m_dirLightRegisters.resize(m_numDirectionalLights * 3);
}

void SuperShaderCompiler::calculateDependencies()
{
	ShaderCompiler::calculateDependencies();

	m_dependencyCounter.addWorldSpaceDependencies(true);
}

void SuperShaderCompiler::compileNormalCode()
{
	m_sharedRegisters.m_normalFragment = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(m_sharedRegisters.m_normalFragment, m_dependencyCounter.getNormalDependencies());

	if (m_methodSetup->m_normalMethod->hasOutput() && !m_methodSetup->m_normalMethod->isTangentSpace())
	{
		m_methodSetup->m_normalMethod->getVertexCode(m_vertexCode, m_methodSetup->m_normalMethodVO, &m_registerCache);
		m_methodSetup->m_normalMethod->getFragmentCode(m_fragmentLightCode, m_methodSetup->m_normalMethodVO, &m_registerCache, m_sharedRegisters.m_normalFragment);
		return;
	}

	m_sharedRegisters.m_normalVarying = m_registerCache.getFreeVarying();
	unsigned int normalMatrixReg = m_registerCache.getFreeVertexConstant();
	m_registerCache.getFreeVertexConstant();
	m_registerCache.getFreeVertexConstant();
	m_registerCache.getFreeVertexConstant();
	m_sceneNormalMatrixIndex = REGISTER_INDEX(normalMatrixReg) * 4;

	if (m_methodSetup->m_normalMethod->hasOutput())
	{
		// tangent stream required
		compileTangentVertexCode(normalMatrixReg);
		compileTangentNormalMapFragmentCode();
	}
	else
	{
		m_vertexCode.m33(m_sharedRegisters.m_normalVarying ^ Regs::xyz, m_sharedRegisters.m_animatedNormal, normalMatrixReg);
		m_vertexCode.mov(m_sharedRegisters.m_normalVarying ^ Regs::w, m_sharedRegisters.m_animatedNormal ^ Regs::w);

		m_fragmentLightCode.nrm(m_sharedRegisters.m_normalFragment ^ Regs::xyz, m_sharedRegisters.m_normalVarying);
		m_fragmentLightCode.mov(m_sharedRegisters.m_normalFragment ^ Regs::w, m_sharedRegisters.m_normalVarying ^ Regs::w);

		if (m_dependencyCounter.getTangentDependencies() > 0)
		{
			m_sharedRegisters.m_tangentInput = m_registerCache.getFreeVertexAttribute();
			m_tangentBufferIndex = REGISTER_INDEX(m_sharedRegisters.m_tangentInput);
			m_sharedRegisters.m_tangentVarying = m_registerCache.getFreeVarying();
			m_vertexCode.mov(m_sharedRegisters.m_tangentVarying, m_sharedRegisters.m_tangentInput);
		}
	}

	m_registerCache.removeVertexTempUsage(m_sharedRegisters.m_animatedNormal);
}

void SuperShaderCompiler::createNormalRegisters()
{
	if (m_dependencyCounter.getNormalDependencies() > 0)
	{
		m_sharedRegisters.m_normalInput = m_registerCache.getFreeVertexAttribute();
		m_normalBufferIndex = REGISTER_INDEX(m_sharedRegisters.m_normalInput);
		m_sharedRegisters.m_animatedNormal = m_registerCache.getFreeVertexVectorTemp();
		m_registerCache.addVertexTempUsages(m_sharedRegisters.m_animatedNormal, 1);
		m_animatableAttributes.push_back(m_sharedRegisters.m_normalInput);
		m_animationTargetRegisters.push_back(m_sharedRegisters.m_animatedNormal);
	}

	if (m_methodSetup->m_normalMethod->hasOutput())
	{
		m_sharedRegisters.m_tangentInput = m_registerCache.getFreeVertexAttribute();
		m_tangentBufferIndex = REGISTER_INDEX(m_sharedRegisters.m_tangentInput);

		m_sharedRegisters.m_animatedTangent = m_registerCache.getFreeVertexVectorTemp();
		m_registerCache.addVertexTempUsages(m_sharedRegisters.m_animatedTangent, 1);

		m_animatableAttributes.push_back(m_sharedRegisters.m_tangentInput);
		m_animationTargetRegisters.push_back(m_sharedRegisters.m_animatedTangent);
	}
}

void SuperShaderCompiler::compileViewDirCode()
{
	unsigned int cameraPositionReg = m_registerCache.getFreeVertexConstant();
	m_sharedRegisters.m_viewDirVarying = m_registerCache.getFreeVarying();
	m_sharedRegisters.m_viewDirFragment = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(m_sharedRegisters.m_viewDirFragment, m_dependencyCounter.getViewDirDependencies());
	m_cameraPositionIndex = REGISTER_INDEX(cameraPositionReg) * 4;

	m_vertexCode.sub(m_sharedRegisters.m_viewDirVarying, cameraPositionReg, m_sharedRegisters.m_globalPositionVertex);
	m_fragmentLightCode.nrm(m_sharedRegisters.m_viewDirFragment ^ Regs::xyz, m_sharedRegisters.m_viewDirVarying);
	m_fragmentLightCode.mov(m_sharedRegisters.m_viewDirFragment ^ Regs::w, m_sharedRegisters.m_viewDirVarying ^ Regs::w);

	m_registerCache.removeVertexTempUsage(m_sharedRegisters.m_globalPositionVertex);
}

void SuperShaderCompiler::compileLightingCode()
{
	m_sharedRegisters.m_shadedTarget = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(m_sharedRegisters.m_shadedTarget, 1);

	m_methodSetup->m_diffuseMethod->getVertexCode(m_vertexCode, m_methodSetup->m_diffuseMethodVO, &m_registerCache);
	m_methodSetup->m_diffuseMethod->getFragmentPreLightingCode(m_fragmentLightCode, m_methodSetup->m_diffuseMethodVO, &m_registerCache);

	if (m_usingSpecularMethod)
	{
		m_methodSetup->m_specularMethod->getVertexCode(m_vertexCode, m_methodSetup->m_specularMethodVO, &m_registerCache);
		m_methodSetup->m_specularMethod->getFragmentPreLightingCode(m_fragmentLightCode, m_methodSetup->m_specularMethodVO, &m_registerCache);
	}

	if (usesLights())
	{
		initLightRegisters();
		compileDirectionalLightCode();
		compilePointLightCode();
	}

	if (usesProbes())
		compileLightProbeCode();

	// only need to create and reserve _shadedTargetReg here, no earlier?
	m_methodSetup->m_ambientMethod->getVertexCode(m_vertexCode, m_methodSetup->m_ambientMethodVO, &m_registerCache);
	m_methodSetup->m_ambientMethod->getFragmentCode(m_fragmentLightCode, m_methodSetup->m_ambientMethodVO, &m_registerCache, m_sharedRegisters.m_shadedTarget);

	if (m_methodSetup->m_ambientMethodVO->m_needsNormals)
		m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_normalFragment);

	if (m_methodSetup->m_ambientMethodVO->m_needsView)
		m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_viewDirFragment);

	unsigned int shadowReg = 0;
	if (m_methodSetup->m_shadowMethod)
	{
		m_methodSetup->m_shadowMethod->getVertexCode(m_vertexCode, m_methodSetup->m_shadowMethodVO, &m_registerCache);

		// using normal to contain shadow data if available is perhaps risky :s
		// todo: improve compilation with lifetime analysis so this isn't necessary?
		if (m_dependencyCounter.getNormalDependencies() == 0)
		{
			shadowReg = m_registerCache.getFreeFragmentVectorTemp();
			m_registerCache.addFragmentTempUsages(shadowReg, 1);
		}
		else
			shadowReg = m_sharedRegisters.m_normalFragment;

		m_methodSetup->m_diffuseMethod->setShadowRegister(shadowReg);
		m_methodSetup->m_shadowMethod->getFragmentCode(m_fragmentLightCode, m_methodSetup->m_shadowMethodVO, &m_registerCache, shadowReg);
	}

	m_methodSetup->m_diffuseMethod->getFragmentPostLightingCode(m_fragmentLightCode, m_methodSetup->m_diffuseMethodVO, &m_registerCache, m_sharedRegisters.m_shadedTarget);

	if (m_alphaPremultiplied)
	{
		m_fragmentLightCode.add(m_sharedRegisters.m_shadedTarget ^ Regs::w, m_sharedRegisters.m_shadedTarget ^ Regs::w, m_sharedRegisters.m_commons ^ Regs::z);
		m_fragmentLightCode.div(m_sharedRegisters.m_shadedTarget ^ Regs::xyz, m_sharedRegisters.m_shadedTarget, m_sharedRegisters.m_shadedTarget ^ Regs::w);
		m_fragmentLightCode.sub(m_sharedRegisters.m_shadedTarget ^ Regs::w, m_sharedRegisters.m_shadedTarget ^ Regs::w, m_sharedRegisters.m_commons ^ Regs::z);
		m_fragmentLightCode.sat(m_sharedRegisters.m_shadedTarget ^ Regs::xyz, m_sharedRegisters.m_shadedTarget);
	}

	// resolve other dependencies as well?
	if (m_methodSetup->m_diffuseMethodVO->m_needsNormals)
		m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_normalFragment);

	if (m_methodSetup->m_diffuseMethodVO->m_needsView)
		m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_viewDirFragment);

	if (m_usingSpecularMethod)
	{
		m_methodSetup->m_specularMethod->setShadowRegister(shadowReg);
		m_methodSetup->m_specularMethod->getFragmentPostLightingCode(m_fragmentLightCode, m_methodSetup->m_specularMethodVO, &m_registerCache, m_sharedRegisters.m_shadedTarget);

		if (m_methodSetup->m_specularMethodVO->m_needsNormals)
			m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_normalFragment);

		if (m_methodSetup->m_specularMethodVO->m_needsView)
			m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_viewDirFragment);
	}
}

void SuperShaderCompiler::compileTangentVertexCode(unsigned int matrixReg)
{
	m_sharedRegisters.m_tangentVarying = m_registerCache.getFreeVarying();
	m_sharedRegisters.m_bitangentVarying = m_registerCache.getFreeVarying();

	m_vertexCode.m33(m_sharedRegisters.m_animatedNormal ^ Regs::xyz, m_sharedRegisters.m_animatedNormal, matrixReg);
	m_vertexCode.nrm(m_sharedRegisters.m_animatedNormal ^ Regs::xyz, m_sharedRegisters.m_animatedNormal);

	m_vertexCode.m33(m_sharedRegisters.m_animatedTangent ^ Regs::xyz, m_sharedRegisters.m_animatedTangent, matrixReg);
	m_vertexCode.nrm(m_sharedRegisters.m_animatedTangent ^ Regs::xyz, m_sharedRegisters.m_animatedTangent);

	unsigned int bitanTemp = m_registerCache.getFreeVertexVectorTemp();
	m_vertexCode.mov(m_sharedRegisters.m_tangentVarying ^ Regs::x, m_sharedRegisters.m_animatedTangent ^ Regs::x);
	m_vertexCode.mov(m_sharedRegisters.m_tangentVarying ^ Regs::z, m_sharedRegisters.m_animatedNormal ^ Regs::x);
	m_vertexCode.mov(m_sharedRegisters.m_tangentVarying ^ Regs::w, m_sharedRegisters.m_normalInput ^ Regs::w);
	m_vertexCode.mov(m_sharedRegisters.m_bitangentVarying ^ Regs::x, m_sharedRegisters.m_animatedTangent ^ Regs::y);
	m_vertexCode.mov(m_sharedRegisters.m_bitangentVarying ^ Regs::z, m_sharedRegisters.m_animatedNormal ^ Regs::y);
	m_vertexCode.mov(m_sharedRegisters.m_bitangentVarying ^ Regs::w, m_sharedRegisters.m_normalInput ^ Regs::w);
	m_vertexCode.mov(m_sharedRegisters.m_normalVarying ^ Regs::x, m_sharedRegisters.m_animatedTangent ^ Regs::z);
	m_vertexCode.mov(m_sharedRegisters.m_normalVarying ^ Regs::z, m_sharedRegisters.m_animatedNormal ^ Regs::z);
	m_vertexCode.mov(m_sharedRegisters.m_normalVarying ^ Regs::w, m_sharedRegisters.m_normalInput ^ Regs::w);
	m_vertexCode.crs(bitanTemp ^ Regs::xyz, m_sharedRegisters.m_animatedNormal, m_sharedRegisters.m_animatedTangent);
	m_vertexCode.mov(m_sharedRegisters.m_tangentVarying ^ Regs::y, bitanTemp ^ Regs::x);
	m_vertexCode.mov(m_sharedRegisters.m_bitangentVarying ^ Regs::y, bitanTemp ^ Regs::y);
	m_vertexCode.mov(m_sharedRegisters.m_normalVarying ^ Regs::y, bitanTemp ^ Regs::z);

	m_registerCache.removeVertexTempUsage(m_sharedRegisters.m_animatedTangent);
}

void SuperShaderCompiler::compileTangentNormalMapFragmentCode()
{
	unsigned int t = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(t, 1);
	unsigned int b = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(b, 1);
	unsigned int n = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(n, 1);

	m_fragmentLightCode.nrm(t ^ Regs::xyz, m_sharedRegisters.m_tangentVarying);
	m_fragmentLightCode.mov(t ^ Regs::w, m_sharedRegisters.m_tangentVarying ^ Regs::w);
	m_fragmentLightCode.nrm(b ^ Regs::xyz, m_sharedRegisters.m_bitangentVarying);
	m_fragmentLightCode.nrm(n ^ Regs::xyz, m_sharedRegisters.m_normalVarying);

	unsigned int temp = m_registerCache.getFreeFragmentVectorTemp();
	m_registerCache.addFragmentTempUsages(temp, 1);
	m_methodSetup->m_normalMethod->getFragmentCode(m_fragmentLightCode, m_methodSetup->m_normalMethodVO, &m_registerCache, temp);
	m_fragmentLightCode.m33(m_sharedRegisters.m_normalFragment ^ Regs::xyz, temp, t);
	m_fragmentLightCode.mov(m_sharedRegisters.m_normalFragment ^ Regs::w, m_sharedRegisters.m_normalVarying ^ Regs::w);
	m_registerCache.removeFragmentTempUsage(temp);

	if (m_methodSetup->m_normalMethodVO->m_needsView)
		m_registerCache.removeFragmentTempUsage(m_sharedRegisters.m_viewDirFragment);

	if (m_methodSetup->m_normalMethodVO->m_needsGlobalVertexPos || m_methodSetup->m_normalMethodVO->m_needsGlobalFragmentPos)
		m_registerCache.removeVertexTempUsage(m_sharedRegisters.m_globalPositionVertex);

	m_registerCache.removeFragmentTempUsage(b);
	m_registerCache.removeFragmentTempUsage(t);
	m_registerCache.removeFragmentTempUsage(n);
}

void SuperShaderCompiler::initLightRegisters()
{
	for (auto& dirLightReg : m_dirLightRegisters)
	{
		dirLightReg = m_registerCache.getFreeFragmentConstant();
		if (m_lightFragmentConstantIndex == -1)
			m_lightFragmentConstantIndex = REGISTER_INDEX(dirLightReg) * 4;
	}

	for (auto& pointLightReg : m_pointLightRegisters)
	{
		pointLightReg = m_registerCache.getFreeFragmentConstant();
		if (m_lightFragmentConstantIndex == -1)
			m_lightFragmentConstantIndex = REGISTER_INDEX(pointLightReg) * 4;
	}
}

void SuperShaderCompiler::compileDirectionalLightCode()
{
	bool addSpec = m_usingSpecularMethod && usesLightsForSpecular();
	bool addDiff = usesLightsForDiffuse();
	if (!(addSpec || addDiff))
		return;

	for (int i = 0, regIndex = 0; i < m_numDirectionalLights; i++)
	{
		unsigned int lightDirReg = m_dirLightRegisters[regIndex++];
		unsigned int diffuseColorReg = m_dirLightRegisters[regIndex++];
		unsigned int specularColorReg = m_dirLightRegisters[regIndex++];

		if (addDiff)
			m_methodSetup->m_diffuseMethod->getFragmentCodePerLight(m_fragmentLightCode, m_methodSetup->m_diffuseMethodVO, lightDirReg, diffuseColorReg, &m_registerCache);

		if (addSpec)
			m_methodSetup->m_specularMethod->getFragmentCodePerLight(m_fragmentLightCode, m_methodSetup->m_specularMethodVO, lightDirReg, specularColorReg, &m_registerCache);
	}
}

void SuperShaderCompiler::compilePointLightCode()
{
	bool addSpec = m_usingSpecularMethod && usesLightsForSpecular();
	bool addDiff = usesLightsForDiffuse();
	if (!(addSpec || addDiff))
		return;

	for (int i = 0, regIndex = 0; i < m_numPointLights; i++)
	{
		unsigned int lightPosReg = m_pointLightRegisters[regIndex++];
		unsigned int diffuseColorReg = m_pointLightRegisters[regIndex++];
		unsigned int specularColorReg = m_pointLightRegisters[regIndex++];
		unsigned int lightDirReg = m_registerCache.getFreeFragmentVectorTemp();
		m_registerCache.addFragmentTempUsages(lightDirReg, 1);

		// calculate attenuation
		m_fragmentLightCode.sub(lightDirReg, lightPosReg, m_sharedRegisters.m_globalPositionVarying);
		m_fragmentLightCode.dp3(lightDirReg ^ Regs::w, lightDirReg, lightDirReg);
		m_fragmentLightCode.sub(lightDirReg ^ Regs::w, lightDirReg ^ Regs::w, diffuseColorReg ^ Regs::w);
		m_fragmentLightCode.mul(lightDirReg ^ Regs::w, lightDirReg ^ Regs::w, specularColorReg ^ Regs::w);
		m_fragmentLightCode.sat(lightDirReg ^ Regs::w, lightDirReg ^ Regs::w);
		m_fragmentLightCode.sub(lightDirReg ^ Regs::w, lightPosReg ^ Regs::w, lightDirReg ^ Regs::w);
		m_fragmentLightCode.nrm(lightDirReg ^ Regs::xyz, lightDirReg);

		if (m_lightFragmentConstantIndex == -1)
			m_lightFragmentConstantIndex = REGISTER_INDEX(lightPosReg) * 4;

		if (addDiff)
			m_methodSetup->m_diffuseMethod->getFragmentCodePerLight(m_fragmentLightCode, m_methodSetup->m_diffuseMethodVO, lightDirReg, diffuseColorReg, &m_registerCache);

		if (addSpec)
			m_methodSetup->m_specularMethod->getFragmentCodePerLight(m_fragmentLightCode, m_methodSetup->m_specularMethodVO, lightDirReg, specularColorReg, &m_registerCache);

		m_registerCache.removeFragmentTempUsage(lightDirReg);
	}
}

void SuperShaderCompiler::compileLightProbeCode()
{
	bool addSpec = m_usingSpecularMethod && usesProbesForSpecular();
	bool addDiff = usesProbesForDiffuse();
	if (!(addSpec || addDiff))
		return;

	std::vector<unsigned int> weightRegisters;
	weightRegisters.resize(m_numProbeRegisters);

	for (int i = 0; i < m_numProbeRegisters; i++)
	{
		weightRegisters[i] = m_registerCache.getFreeFragmentConstant();
		if (i == 0)
			m_probeWeightsIndex = REGISTER_INDEX(weightRegisters[i]) * 4;
	}

	unsigned int weightComponents[4] = { Regs::x, Regs::y, Regs::z, Regs::w };
	for (int i = 0; i < m_numLightProbes; i++)
	{
		unsigned int weightReg = weightRegisters[i / 4] ^ weightComponents[i % 4];

		if (addDiff)
		{
			unsigned int texReg = m_registerCache.getFreeTextureReg();
			m_lightProbeDiffuseIndices[i] = REGISTER_INDEX(texReg);
			m_methodSetup->m_diffuseMethod->getFragmentCodePerProbe(m_fragmentLightCode, m_methodSetup->m_diffuseMethodVO, texReg, weightReg, &m_registerCache);
		}

		if (addSpec)
		{
			unsigned int texReg = m_registerCache.getFreeTextureReg();
			m_lightProbeSpecularIndices[i] = REGISTER_INDEX(texReg);
			m_methodSetup->m_specularMethod->getFragmentCodePerProbe(m_fragmentLightCode, m_methodSetup->m_specularMethodVO, texReg, weightReg, &m_registerCache);
		}
	}
}