#include "CompiledPass.h"
#include "ShaderMethodSetup.h"
#include "BasicNormalMethod.h"
#include "BasicAmbientMethod.h"
#include "ShadowMapMethodBase.h"
#include "BasicDiffuseMethod.h"
#include "BasicSpecularMethod.h"
#include "ColorTransformMethod.h"
#include "IRenderable.h"
#include "IContext.h"
#include "Shader.h"
#include "Matrix.h"
#include "Matrix3D.h"
#include "MethodVOSet.h"
#include "ShadingMethodEvent.h"
#include "ShaderCompiler.h"

USING_AWAY_NAMESPACE

CompiledPass::CompiledPass(MaterialBase* material)
{
	m_material = material;
	m_passesDirty = false;
	m_specularLightSources = 1;
	m_diffuseLightSources = 3;
	m_animateUVs = false;
	m_numPointLights = 0;
	m_numDirectionalLights = 0;
	m_numLightProbes = 0;
	m_enableLightFallOff = true;
	m_passesDirty = false;
	m_forceSeparateMVP = false;

	m_methodSetup = new ShaderMethodSetup();
	m_methodSetup->addEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
}

CompiledPass::~CompiledPass()
{
	m_methodSetup->removeEventListener(ShadingMethodEvent::SHADER_INVALIDATED, CALLBACK_OBJECT(onShaderInvalidated));
	delete m_methodSetup;
}

void CompiledPass::setLightFallOff(bool value)
{
	if (value != m_enableLightFallOff)
	{
		m_enableLightFallOff = value;
		invalidateShaderProgram(true);
	}
}

void CompiledPass::setAnimateUVs(bool value)
{
	if (value != m_animateUVs)
	{
		m_animateUVs = value;
		invalidateShaderProgram();
	}
}

Texture2DBase* CompiledPass::getNormalMap()
{
	return m_methodSetup->m_normalMethod->getNormalMap();
}

void CompiledPass::setNormalMap(Texture2DBase* value)
{
	m_methodSetup->m_normalMethod->setNormalMap(value);
}

BasicNormalMethod* CompiledPass::getNormalMethod()
{
	return m_methodSetup->m_normalMethod;
}

void CompiledPass::setNormalMethod(BasicNormalMethod* value)
{
	m_methodSetup->setNormalMethod(value);
}

BasicAmbientMethod* CompiledPass::getAmbientMethod()
{
	return m_methodSetup->m_ambientMethod;
}

void CompiledPass::setAmbientMethod(BasicAmbientMethod* value)
{
	m_methodSetup->setAmbientMethod(value);
}

ShadowMapMethodBase* CompiledPass::getShadowMethod()
{
	return m_methodSetup->m_shadowMethod;
}

void CompiledPass::setShadowMethod(ShadowMapMethodBase* value)
{
	m_methodSetup->setShadowMethod(value);
}

BasicDiffuseMethod* CompiledPass::getDiffuseMethod()
{
	return m_methodSetup->m_diffuseMethod;
}

void CompiledPass::setDiffuseMethod(BasicDiffuseMethod* value)
{
	m_methodSetup->setDiffuseMethod(value);
}

BasicSpecularMethod* CompiledPass::getSpecularMethod()
{
	return m_methodSetup->m_specularMethod;
}

void CompiledPass::setSpecularMethod(BasicSpecularMethod* value)
{
	m_methodSetup->setSpecularMethod(value);
}

void CompiledPass::updateProgram(IContext* context)
{
	initCompiler();
	updateShaderProperties();
	initConstantData();
	MaterialPassBase::updateProgram(context);
	cleanUp();
}

void CompiledPass::invalidateShaderProgram(bool updateMaterial)
{
	MaterialPassVector oldPasses = m_passes;

	m_passes.clear();
	addPassesFromMethods();

	if (m_passes.size() != oldPasses.size())
	{
		m_passesDirty = true;
		return;
	}

	for (unsigned int i = 0; i < m_passes.size(); i++)
	{
		if (m_passes[i] != oldPasses[i])
		{
			m_passesDirty = true;
			return;
		}
	}

	MaterialPassBase::invalidateShaderProgram(updateMaterial);
}

void CompiledPass::getVertexCode(Shader& vertexShader)
{
	vertexShader.append(m_vertexCode);
}

void CompiledPass::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.prepend(m_fragmentLightCode);
	fragmentShader.append(m_framentPostLightCode);
}

void CompiledPass::activate(IContext* context, Camera3D* camera)
{
	MaterialPassBase::activate(context, camera);

	if (m_usesNormals)
		m_methodSetup->m_normalMethod->activate(m_methodSetup->m_normalMethodVO, context);

	m_methodSetup->m_ambientMethod->activate(m_methodSetup->m_ambientMethodVO, context);

	if (m_methodSetup->m_shadowMethod)
		m_methodSetup->m_shadowMethod->activate(m_methodSetup->m_shadowMethodVO, context);

	m_methodSetup->m_diffuseMethod->activate(m_methodSetup->m_diffuseMethodVO, context);

	if (m_usingSpecularMethod)
		m_methodSetup->m_specularMethod->activate(m_methodSetup->m_specularMethodVO, context);
}

void CompiledPass::deactivate(IContext* context)
{
	MaterialPassBase::deactivate(context);

	if (m_usesNormals)
		m_methodSetup->m_normalMethod->deactivate(m_methodSetup->m_normalMethodVO, context);

	m_methodSetup->m_ambientMethod->deactivate(m_methodSetup->m_ambientMethodVO, context);

	if (m_methodSetup->m_shadowMethod)
		m_methodSetup->m_shadowMethod->deactivate(m_methodSetup->m_shadowMethodVO, context);

	m_methodSetup->m_diffuseMethod->deactivate(m_methodSetup->m_diffuseMethodVO, context);

	if (m_usingSpecularMethod)
		m_methodSetup->m_specularMethod->deactivate(m_methodSetup->m_specularMethodVO, context);
}

void CompiledPass::render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	if (m_uvBufferIndex >= 0)
		renderable->activateUVBuffer(m_uvBufferIndex, context);

	if (m_secondaryUVBufferIndex >= 0)
		renderable->activateSecondaryUVBuffer(m_secondaryUVBufferIndex, context);

	if (m_normalBufferIndex >= 0)
		renderable->activateVertexNormalBuffer(m_normalBufferIndex, context);

	if (m_tangentBufferIndex >= 0)
		renderable->activateVertexTangentBuffer(m_tangentBufferIndex, context);

	if (m_animateUVs)
	{
		Matrix* uvTransform = renderable->getUVTransform();
		if (uvTransform)
		{
			m_vertexConstantData[m_uvTransformIndex] = uvTransform->m_a;
			m_vertexConstantData[m_uvTransformIndex + 1] = uvTransform->m_b;
			m_vertexConstantData[m_uvTransformIndex + 3] = uvTransform->m_tx;
			m_vertexConstantData[m_uvTransformIndex + 4] = uvTransform->m_c;
			m_vertexConstantData[m_uvTransformIndex + 5] = uvTransform->m_d;
			m_vertexConstantData[m_uvTransformIndex + 7] = uvTransform->m_ty;
		}
		else
		{
			m_vertexConstantData[m_uvTransformIndex] = 1;
			m_vertexConstantData[m_uvTransformIndex + 1] = 0;
			m_vertexConstantData[m_uvTransformIndex + 3] = 0;
			m_vertexConstantData[m_uvTransformIndex + 4] = 0;
			m_vertexConstantData[m_uvTransformIndex + 5] = 1;
			m_vertexConstantData[m_uvTransformIndex + 7] = 0;
		}
	}

	m_ambientLightR = m_ambientLightG = m_ambientLightB = 0;

	if (usesLights())
		updateLightConstants();

	if (usesProbes())
		updateProbes(context);

	float* data = m_vertexConstantData.data();
	if (m_sceneMatrixIndex >= 0)
	{
		renderable->getRenderSceneTransform(camera).copyRawDataTo(data, m_sceneMatrixIndex, true);
		viewProjection.copyRawDataTo(data, 0, true);
	}
	else
	{
		Matrix3D matrix;
		matrix.copyFrom(renderable->getRenderSceneTransform(camera));
		matrix.append(viewProjection);
		matrix.copyRawDataTo(data, 0, true);
	}

	if (m_sceneNormalMatrixIndex >= 0)
		renderable->getInverseSceneTransform().copyRawDataTo(data, m_sceneNormalMatrixIndex, false);

	if (m_usesNormals)
		m_methodSetup->m_normalMethod->setRenderState(m_methodSetup->m_normalMethodVO, renderable, context, camera);

	BasicAmbientMethod* ambientMethod = m_methodSetup->m_ambientMethod;
	ambientMethod->m_lightAmbientR = m_ambientLightR;
	ambientMethod->m_lightAmbientG = m_ambientLightG;
	ambientMethod->m_lightAmbientB = m_ambientLightB;
	ambientMethod->setRenderState(m_methodSetup->m_ambientMethodVO, renderable, context, camera);

	if (m_methodSetup->m_shadowMethod)
		m_methodSetup->m_shadowMethod->setRenderState(m_methodSetup->m_shadowMethodVO, renderable, context, camera);

	m_methodSetup->m_diffuseMethod->setRenderState(m_methodSetup->m_diffuseMethodVO, renderable, context, camera);

	if (m_usingSpecularMethod)
		m_methodSetup->m_specularMethod->setRenderState(m_methodSetup->m_specularMethodVO, renderable, context, camera);

	if (m_methodSetup->m_colorTransformMethod)
		m_methodSetup->m_colorTransformMethod->setRenderState(m_methodSetup->m_colorTransformMethodVO, renderable, context, camera);

	for (auto set : m_methodSetup->m_methods)
		set->m_method->setRenderState(set->m_data, renderable, context, camera);

	context->setProgramConstantsFromVector(ProgramType::VERTEX, 0, data, m_numUsedVertexConstants);
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_fragmentConstantData.data(), m_numUsedFragmentConstants);
	renderable->activateVertexBuffer(0, context);
	context->drawTriangles(renderable->getIndexBuffer(context), 0, renderable->getTriangleCount());
}

void CompiledPass::addPassesFromMethods()
{
	if (m_methodSetup->m_normalMethod && m_methodSetup->m_normalMethod->hasOutput())
		addPasses(m_methodSetup->m_normalMethod->getPasses());

	if (m_methodSetup->m_ambientMethod)
		addPasses(m_methodSetup->m_ambientMethod->getPasses());

	if (m_methodSetup->m_shadowMethod)
		addPasses(m_methodSetup->m_shadowMethod->getPasses());

	if (m_methodSetup->m_diffuseMethod)
		addPasses(m_methodSetup->m_diffuseMethod->getPasses());

	if (m_methodSetup->m_specularMethod)
		addPasses(m_methodSetup->m_specularMethod->getPasses());
}

bool CompiledPass::usesProbes()
{
	return m_numLightProbes > 0 && ((m_diffuseLightSources | m_specularLightSources) & static_cast<unsigned int>(LightSources::PROBES)) != 0;
}

bool CompiledPass::usesLights()
{
	return (m_numPointLights > 0 || m_numDirectionalLights > 0) && ((m_diffuseLightSources | m_specularLightSources) & static_cast<unsigned int>(LightSources::LIGHTS)) != 0;
}

void CompiledPass::initCompiler()
{
	m_compiler = createCompiler();
	m_compiler->setForceSeperateMVP(m_forceSeparateMVP);
	m_compiler->setPointLightCount(m_numPointLights);
	m_compiler->setDirectionalLightCount(m_numDirectionalLights);
	m_compiler->setLightProbeCount(m_numLightProbes);
	m_compiler->setMethodSetup(m_methodSetup);
	m_compiler->setDiffuseLightSources(m_diffuseLightSources);
	m_compiler->setSpecularLightSources(m_specularLightSources);
	m_compiler->setTextureSampling(m_smooth, m_repeat, m_mipmap);
	m_compiler->setConstantDataBuffers(&m_vertexConstantData, &m_fragmentConstantData);
	m_compiler->setAnimateUVs(m_animateUVs);
	m_compiler->setAlphaPremultiplied(m_alphaPremultiplied && m_enableBlending);
	m_compiler->setLightFallOff(m_enableLightFallOff);
	m_compiler->compile();
}

void CompiledPass::updateShaderProperties()
{
	m_animatableAttributes = m_compiler->getAnimatableAttributes();
	m_animationTargetRegisters = m_compiler->getAnimationTargetRegisters();
	m_vertexCode = m_compiler->getVertexCode();
	m_fragmentLightCode = m_compiler->getFragmentLightCode();
	m_framentPostLightCode = m_compiler->getFragmentPostLightCode();
	m_shadedTarget = m_compiler->getShadedTarget();
	m_usingSpecularMethod = m_compiler->usingSpecularMethod();
	m_usesNormals = m_compiler->usesNormals();
	m_needUVAnimation = m_compiler->needUVAnimation();
	m_uvSource = m_compiler->getUVSource();
	m_uvTarget = m_compiler->getUVTarget();

	updateRegisterIndices();
	updateUsedOffsets();
}

void CompiledPass::updateRegisterIndices()
{
	m_uvBufferIndex = m_compiler->getUVBufferIndex();
	m_uvTransformIndex = m_compiler->getUVTransformIndex();
	m_secondaryUVBufferIndex = m_compiler->getSecondaryUVBufferIndex();
	m_normalBufferIndex = m_compiler->getNormalBufferIndex();
	m_tangentBufferIndex = m_compiler->getTangentBufferIndex();
	m_lightFragmentConstantIndex = m_compiler->getLightFragmentConstantIndex();
	m_cameraPositionIndex = m_compiler->getCameraPositionIndex();
	m_commonsDataIndex = m_compiler->getCommonsDataIndex();
	m_sceneMatrixIndex = m_compiler->getSceneMatrixIndex();
	m_sceneNormalMatrixIndex = m_compiler->getSceneNormalMatrixIndex();
	m_probeWeightsIndex = m_compiler->getProbeWeightsIndex();
	m_lightProbeDiffuseIndices = m_compiler->getLightProbeDiffuseIndices();
	m_lightProbeSpecularIndices = m_compiler->getLightProbeSpecularIndices();
}

void CompiledPass::addPasses(MaterialPassVector& passes)
{
	for (auto pass : passes)
	{
		pass->setMaterial(m_material);
		pass->setLightPicker(m_lightPicker);
		m_passes.push_back(pass);
	}
}

void CompiledPass::initUVTransformData()
{
	m_vertexConstantData[m_uvTransformIndex] = 1;
	m_vertexConstantData[m_uvTransformIndex + 1] = 0;
	m_vertexConstantData[m_uvTransformIndex + 2] = 0;
	m_vertexConstantData[m_uvTransformIndex + 3] = 0;
	m_vertexConstantData[m_uvTransformIndex + 4] = 0;
	m_vertexConstantData[m_uvTransformIndex + 5] = 1;
	m_vertexConstantData[m_uvTransformIndex + 6] = 0;
	m_vertexConstantData[m_uvTransformIndex + 7] = 0;
}

void CompiledPass::initCommonsData()
{
	m_fragmentConstantData[m_commonsDataIndex] = .5f;
	m_fragmentConstantData[m_commonsDataIndex + 1] = 0;
	m_fragmentConstantData[m_commonsDataIndex + 2] = 1.f / 255.f;
	m_fragmentConstantData[m_commonsDataIndex + 3] = 1;
}

void CompiledPass::cleanUp()
{
	delete m_compiler;
	m_compiler = nullptr;
}

void CompiledPass::updateMethodConstants()
{
	if (m_methodSetup->m_normalMethod)
		m_methodSetup->m_normalMethod->initConstants(m_methodSetup->m_normalMethodVO);

	if (m_methodSetup->m_diffuseMethod)
		m_methodSetup->m_diffuseMethod->initConstants(m_methodSetup->m_diffuseMethodVO);

	if (m_methodSetup->m_ambientMethod)
		m_methodSetup->m_ambientMethod->initConstants(m_methodSetup->m_ambientMethodVO);

	if (m_usingSpecularMethod)
		m_methodSetup->m_specularMethod->initConstants(m_methodSetup->m_specularMethodVO);

	if (m_methodSetup->m_shadowMethod)
		m_methodSetup->m_shadowMethod->initConstants(m_methodSetup->m_shadowMethodVO);
}

void CompiledPass::updateLightConstants()
{

}

void CompiledPass::updateProbes(IContext* context)
{

}

void CompiledPass::updateUsedOffsets()
{
	m_numUsedVertexConstants = m_compiler->getUsedVertexConstantCount();
	m_numUsedFragmentConstants = m_compiler->getUsedFragmentConstantCount();
	m_numUsedStreams = m_compiler->getUsedStreamCount();
	m_numUsedTextures = m_compiler->getUsedTextureCount();
	m_numUsedVaryings = m_compiler->getUsedVaryingCount();
}

void CompiledPass::initConstantData()
{
	m_vertexConstantData.resize(m_numUsedVertexConstants * 4);
	m_fragmentConstantData.resize(m_numUsedFragmentConstants * 4);

	initCommonsData();

	if (m_uvTransformIndex >= 0)
		initUVTransformData();

	if (m_cameraPositionIndex >= 0)
		m_vertexConstantData[m_cameraPositionIndex + 3] = 1;

	updateMethodConstants();
}

void CompiledPass::onShaderInvalidated(Event* event)
{
	invalidateShaderProgram();
}