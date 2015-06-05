#include "SuperShaderPass.h"
#include "SuperShaderCompiler.h"
#include "MethodVOSet.h"
#include "Camera3D.h"
#include "LightPickerBase.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "LightProbe.h"
#include "CubeTextureBase.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

SuperShaderPass::SuperShaderPass(MaterialBase* material) : CompiledPass(material)
{
	m_includeCasters = true;
	m_needFragmentAnimation = true;
}

void SuperShaderPass::setIncludeCasters(bool value)
{
	if (value != m_includeCasters)
	{
		m_includeCasters = value;
		invalidateShaderProgram();
	}
}

void SuperShaderPass::setColorTransform(ColorTransform* value)
{
	if (value)
	{
		if (!m_methodSetup->m_colorTransformMethod)
			m_methodSetup->setColorTransformMethod(new ColorTransformMethod());

		m_methodSetup->m_colorTransformMethod->setColorTransform(value);
	}
	else
	{
		if (m_methodSetup->m_colorTransformMethod)
			m_methodSetup->setColorTransformMethod(nullptr);
	}
}

void SuperShaderPass::activate(IContext* context, Camera3D* camera)
{
	CompiledPass::activate(context, camera);

	if (m_methodSetup->m_colorTransformMethod)
		m_methodSetup->m_colorTransformMethod->activate(m_methodSetup->m_colorTransformMethodVO, context);

	for (auto set : m_methodSetup->m_methods)
		set->m_method->activate(set->m_data, context);

	if (m_cameraPositionIndex >= 0)
	{
		Vector3D* pos = camera->getScenePosition();
		m_vertexConstantData[m_cameraPositionIndex] = pos->m_x;
		m_vertexConstantData[m_cameraPositionIndex + 1] = pos->m_y;
		m_vertexConstantData[m_cameraPositionIndex + 2] = pos->m_z;
	}
}

void SuperShaderPass::deactivate(IContext* context)
{
	CompiledPass::deactivate(context);

	if (m_methodSetup->m_colorTransformMethod)
		m_methodSetup->m_colorTransformMethod->deactivate(m_methodSetup->m_colorTransformMethodVO, context);

	for (auto set : m_methodSetup->m_methods)
		set->m_method->deactivate(set->m_data, context);
}

ShaderCompiler* SuperShaderPass::createCompiler()
{
	return new SuperShaderCompiler();
}

void SuperShaderPass::updateLights()
{
	if (m_lightPicker)
	{
		m_numPointLights = m_lightPicker->getPointLightCount();
		m_numDirectionalLights = m_lightPicker->getDirectionalLightCount();
		m_numLightProbes = m_lightPicker->getLightProbeCount();

		if (m_includeCasters)
		{
			m_numPointLights += m_lightPicker->getCastingPointLightCount();
			m_numDirectionalLights += m_lightPicker->getCastingDirectionalLightCount();
		}
	}
	else
	{
		m_numPointLights = 0;
		m_numDirectionalLights = 0;
		m_numLightProbes = 0;
	}

	invalidateShaderProgram();
}

void SuperShaderPass::addPassesFromMethods()
{
	CompiledPass::addPassesFromMethods();

	if (m_methodSetup->m_colorTransformMethod)
		addPasses(m_methodSetup->m_colorTransformMethod->getPasses());

	for (auto set : m_methodSetup->m_methods)
		addPasses(set->m_method->getPasses());
}

void SuperShaderPass::updateMethodConstants()
{
	CompiledPass::updateMethodConstants();

	if (m_methodSetup->m_colorTransformMethod)
		m_methodSetup->m_colorTransformMethod->initConstants(m_methodSetup->m_colorTransformMethodVO);

	for (auto set : m_methodSetup->m_methods)
		set->m_method->initConstants(set->m_data);
}

void SuperShaderPass::updateLightConstants()
{
	int numLightTypes = m_includeCasters ? 2 : 1;
	int i, k = m_lightFragmentConstantIndex, total = 0;
	for (int cast = 0; cast < numLightTypes; cast++)
	{
		DirectionalLightVector& dirLights = cast ? m_lightPicker->getCastingDirectionalLights() : m_lightPicker->getDirectionalLights();
		total += dirLights.size();

		for (auto dirLight : dirLights)
		{
			Vector3D* dirPos = dirLight->getSceneDirection();

			m_ambientLightR += dirLight->getAmbientR();
			m_ambientLightG += dirLight->getAmbientG();
			m_ambientLightB += dirLight->getAmbientB();

			m_fragmentConstantData[k++] = -dirPos->m_x;
			m_fragmentConstantData[k++] = -dirPos->m_y;
			m_fragmentConstantData[k++] = -dirPos->m_z;
			m_fragmentConstantData[k++] = 1;

			m_fragmentConstantData[k++] = dirLight->getDiffuseR();
			m_fragmentConstantData[k++] = dirLight->getDiffuseG();
			m_fragmentConstantData[k++] = dirLight->getDiffuseB();
			m_fragmentConstantData[k++] = 1;

			m_fragmentConstantData[k++] = dirLight->getSpecularR();
			m_fragmentConstantData[k++] = dirLight->getSpecularG();
			m_fragmentConstantData[k++] = dirLight->getSpecularB();
			m_fragmentConstantData[k++] = 1;
		}
	}

	// more directional supported than currently picked, need to clamp all to 0
	if (m_numDirectionalLights > total)
	{
		i = k + (m_numDirectionalLights - total) * 12;
		while (k < i)
			m_fragmentConstantData[k++] = 0;
	}

	total = 0;
	for (int cast = 0; cast < numLightTypes; cast++)
	{
		PointLightVector& pointLights = cast ? m_lightPicker->getCastingPointLights() : m_lightPicker->getPointLights();
		for (auto pointLight : pointLights)
		{
			Vector3D* dirPos = pointLight->getScenePosition();

			m_ambientLightR += pointLight->getAmbientR();
			m_ambientLightG += pointLight->getAmbientG();
			m_ambientLightB += pointLight->getAmbientB();

			m_fragmentConstantData[k++] = dirPos->m_x;
			m_fragmentConstantData[k++] = dirPos->m_y;
			m_fragmentConstantData[k++] = dirPos->m_z;
			m_fragmentConstantData[k++] = 1;

			m_fragmentConstantData[k++] = pointLight->getDiffuseR();
			m_fragmentConstantData[k++] = pointLight->getDiffuseG();
			m_fragmentConstantData[k++] = pointLight->getDiffuseB();
			m_fragmentConstantData[k++] = pointLight->getRadius() * pointLight->getRadius();

			m_fragmentConstantData[k++] = pointLight->getSpecularR();
			m_fragmentConstantData[k++] = pointLight->getSpecularG();
			m_fragmentConstantData[k++] = pointLight->getSpecularB();
			m_fragmentConstantData[k++] = pointLight->getFallOffFactor();
		}
	}

	// more directional supported than currently picked, need to clamp all to 0
	if (m_numPointLights > total)
	{
		i = k + (total - m_numPointLights) * 12;
		for (; k < i; k++)
			m_fragmentConstantData[k] = 0;
	}
}

void SuperShaderPass::updateProbes(IContext* context)
{
	bool addDiff = usesProbesForDiffuse();
	bool addSpec = m_methodSetup->m_specularMethod && usesProbesForSpecular();
	if (!(addDiff || addSpec))
		return;

	LightProbeVector& lightProbes = m_lightPicker->getLightProbes();
	unsigned int len = lightProbes.size();
	for (unsigned int i = 0; i < len; i++)
	{
		LightProbe* probe = lightProbes[i];
		if (addDiff)
		{
			context->setTextureAt((*m_lightProbeDiffuseIndices)[i], probe->getDiffuseMap()->getTextureForContext(context));
		}
		if (addSpec)
		{
			context->setTextureAt((*m_lightProbeSpecularIndices)[i], probe->getSpecularMap()->getTextureForContext(context));
		}
	}

	WeightVector& weights = m_lightPicker->getLightProbeWeights();
	m_fragmentConstantData[m_probeWeightsIndex] = weights[0];
	m_fragmentConstantData[m_probeWeightsIndex + 1] = weights[1];
	m_fragmentConstantData[m_probeWeightsIndex + 2] = weights[2];
	m_fragmentConstantData[m_probeWeightsIndex + 3] = weights[3];
}

bool SuperShaderPass::usesProbesForSpecular()
{
	return m_numLightProbes > 0 && (m_specularLightSources & static_cast<unsigned int>(LightSources::PROBES)) != 0;
}

bool SuperShaderPass::usesProbesForDiffuse()
{
	return m_numLightProbes > 0 && (m_diffuseLightSources & static_cast<unsigned int>(LightSources::PROBES)) != 0;
}