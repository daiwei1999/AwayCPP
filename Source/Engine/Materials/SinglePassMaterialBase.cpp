#include "SinglePassMaterialBase.h"
#include "SuperShaderPass.h"
#include "DepthMapPass.h"
#include "DistanceMapPass.h"
#include "BasicDiffuseMethod.h"
#include "ColorTransform.h"
#include "BasicSpecularMethod.h"
#include "BasicAmbientMethod.h"

USING_AWAY_NAMESPACE

SinglePassMaterialBase::SinglePassMaterialBase()
{
	m_alphaBlending = false;
	addPass(m_screenPass = new SuperShaderPass(this));
}

SinglePassMaterialBase::~SinglePassMaterialBase()
{
	delete m_screenPass;
}

bool SinglePassMaterialBase::getLightFallOff()
{
	return m_screenPass->getLightFallOff();
}

void SinglePassMaterialBase::setLightFallOff(bool value)
{
	m_screenPass->setLightFallOff(value);
}

float SinglePassMaterialBase::getAlphaThreshold()
{
	return m_screenPass->getDiffuseMethod()->getAlphaThreshold();
}

void SinglePassMaterialBase::setAlphaThreshold(float value)
{
	m_screenPass->getDiffuseMethod()->setAlphaThreshold(value);
	m_depthPass->setAlphaThreshold(value);
	m_distancePass->setAlphaThreshold(value);
}

unsigned int SinglePassMaterialBase::getSpecularLightSources()
{
	return m_screenPass->getSpecularLightSources();
}

void SinglePassMaterialBase::setSpecularLightSources(unsigned int value)
{
	m_screenPass->setSpecularLightSources(value);
}

unsigned int SinglePassMaterialBase::getDiffuseLightSources()
{
	return m_screenPass->getDiffuseLightSources();
}

void SinglePassMaterialBase::setDiffuseLightSources(unsigned int value)
{
	m_screenPass->setDiffuseLightSources(value);
}

ColorTransform* SinglePassMaterialBase::getColorTransform()
{
	return m_screenPass->getColorTransform();
}

void SinglePassMaterialBase::setColorTransform(ColorTransform* value)
{
	m_screenPass->setColorTransform(value);
}

BasicAmbientMethod* SinglePassMaterialBase::getAmbientMethod()
{
	return m_screenPass->getAmbientMethod();
}

void SinglePassMaterialBase::setAmbientMethod(BasicAmbientMethod* value)
{
	m_screenPass->setAmbientMethod(value);
}

ShadowMapMethodBase* SinglePassMaterialBase::getShadowMethod()
{
	return m_screenPass->getShadowMethod();
}

void SinglePassMaterialBase::setShadowMethod(ShadowMapMethodBase* value)
{
	m_screenPass->setShadowMethod(value);
}

BasicDiffuseMethod* SinglePassMaterialBase::getDiffuseMethod()
{
	return m_screenPass->getDiffuseMethod();
}

void SinglePassMaterialBase::setDiffuseMethod(BasicDiffuseMethod* value)
{
	m_screenPass->setDiffuseMethod(value);
}

BasicNormalMethod* SinglePassMaterialBase::getNormalMethod()
{
	return m_screenPass->getNormalMethod();
}

void SinglePassMaterialBase::setNormalMethod(BasicNormalMethod* value)
{
	m_screenPass->setNormalMethod(value);
}

BasicSpecularMethod* SinglePassMaterialBase::getSpecularMethod()
{
	return m_screenPass->getSpecularMethod();
}

void SinglePassMaterialBase::setSpecularMethod(BasicSpecularMethod* value)
{
	m_screenPass->setSpecularMethod(value);
}

Texture2DBase* SinglePassMaterialBase::getNormalMap()
{
	return m_screenPass->getNormalMap();
}

void SinglePassMaterialBase::setNormalMap(Texture2DBase* value)
{
	m_screenPass->setNormalMap(value);
}

Texture2DBase* SinglePassMaterialBase::getSpecularMap()
{
	return m_screenPass->getSpecularMethod()->getTexture();
}

void SinglePassMaterialBase::setSpecularMap(Texture2DBase* value)
{
	if (m_screenPass->getSpecularMethod())
		m_screenPass->getSpecularMethod()->setTexture(value);
	else
		assert(false); // No specular method was set to assign the specularGlossMap to
}

float SinglePassMaterialBase::getGloss()
{
	return m_screenPass->getSpecularMethod() ? m_screenPass->getSpecularMethod()->getGloss() : 0;
}

void SinglePassMaterialBase::setGloss(float value)
{
	if (m_screenPass->getSpecularMethod())
		m_screenPass->getSpecularMethod()->setGloss(value);
}

float SinglePassMaterialBase::getAmbient()
{
	return m_screenPass->getAmbientMethod()->getAmbient();
}

void SinglePassMaterialBase::setAmbient(float value)
{
	m_screenPass->getAmbientMethod()->setAmbient(value);
}

float SinglePassMaterialBase::getSpecular()
{
	return m_screenPass->getSpecularMethod() ? m_screenPass->getSpecularMethod()->getSpecular() : 0;
}

void SinglePassMaterialBase::setSpecular(float value)
{
	if (m_screenPass->getSpecularMethod())
		m_screenPass->getSpecularMethod()->setSpecular(value);
}

unsigned int SinglePassMaterialBase::getAmbientColor()
{
	return m_screenPass->getAmbientMethod()->getAmbientColor();
}

void SinglePassMaterialBase::setAmbientColor(unsigned int value)
{
	m_screenPass->getAmbientMethod()->setAmbientColor(value);
}

unsigned int SinglePassMaterialBase::getSpecularColor()
{
	return m_screenPass->getSpecularMethod()->getSpecularColor();
}

void SinglePassMaterialBase::setSpecularColor(unsigned int value)
{
	m_screenPass->getSpecularMethod()->setSpecularColor(value);
}

void SinglePassMaterialBase::setAlphaBlending(bool value)
{
	m_alphaBlending = value;
	m_screenPass->setBlendMode(m_blendMode == BlendMode::NORMAL && requiresBlending() ? BlendMode::LAYER : m_blendMode);
}

void SinglePassMaterialBase::setBlendMode(BlendMode value)
{
	MaterialBase::setBlendMode(value);
	m_screenPass->setBlendMode(m_blendMode == BlendMode::NORMAL && requiresBlending() ? BlendMode::LAYER : m_blendMode);
}

void SinglePassMaterialBase::setDepthCompareMode(CompareMode value)
{
	MaterialBase::setDepthCompareMode(value);
	m_screenPass->setDepthCompareMode(value);
}

void SinglePassMaterialBase::activateForDepth(IContext* context, Camera3D* camera, bool distanceBased)
{
	if (distanceBased)
		m_distancePass->setAlphaMask(m_screenPass->getDiffuseMethod()->getTexture());
	else
		m_depthPass->setAlphaMask(m_screenPass->getDiffuseMethod()->getTexture());

	MaterialBase::activateForDepth(context, camera, distanceBased);
}

void SinglePassMaterialBase::updateMaterial(IContext* context)
{
	if (m_screenPass->getPassesDirty())
	{
		clearPasses();

		for (auto pass : m_screenPass->getPasses())
			addPass(pass);

		addPass(m_screenPass);
		m_screenPass->setPassesDirty(false);
	}
}

bool SinglePassMaterialBase::requiresBlending()
{
	return MaterialBase::requiresBlending() || m_alphaBlending || (m_screenPass->getColorTransform() && m_screenPass->getColorTransform()->m_alphaMultiplier < 1);
}

void SinglePassMaterialBase::addMethod(EffectMethodBase* method)
{
	m_screenPass->addMethod(method);
}

void SinglePassMaterialBase::addMethodAt(EffectMethodBase* method, int index)
{
	m_screenPass->addMethodAt(method, index);
}

void SinglePassMaterialBase::removeMethod(EffectMethodBase* method)
{
	m_screenPass->removeMethod(method);
}

void SinglePassMaterialBase::removeAllMethod()
{
	m_screenPass->removeAllMethod();
}

int SinglePassMaterialBase::getMethodCount()
{
	return m_screenPass->getMethodCount();
}

bool SinglePassMaterialBase::hasMethod(EffectMethodBase* method)
{
	return m_screenPass->hasMethod(method);
}

EffectMethodBase* SinglePassMaterialBase::getMethodAt(int index)
{
	return m_screenPass->getMethodAt(index);
}