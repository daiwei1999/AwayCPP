#include "MaterialBase.h"
#include "DepthMapPass.h"
#include "DistanceMapPass.h"
#include "Camera3D.h"
#include "LightPickerBase.h"
#include "IRenderable.h"
#include "EntityCollector.h"
#include "IAnimationSet.h"
#include "IContext.h"
#include "IAnimator.h"
#include "Event.h"

USING_AWAY_NAMESPACE

unsigned int MaterialBase::MATERIAL_ID_COUNT = 0;

MaterialBase::MaterialBase()
{
	m_uniqueId = MATERIAL_ID_COUNT++;
	m_blendMode = BlendMode::NORMAL;
	m_numPasses = 0;
	m_mipmap = true;
	m_smooth = true;
	m_repeat = false;
	m_lightPicker = nullptr;
	m_animationSet = nullptr;
	m_bothSides = false;
	m_alphaPremultiplied = false;
	m_distanceBasedDepthRender = false;
	m_depthCompareMode = CompareMode::LESS_EQUAL;

	m_depthPass = new DepthMapPass();
	m_distancePass = new DistanceMapPass();
	m_depthPass->addEventListener(Event::CHANGE, CALLBACK_OBJECT(onDepthPassChange));
	m_distancePass->addEventListener(Event::CHANGE, CALLBACK_OBJECT(onDistancePassChange));
}

MaterialBase::~MaterialBase()
{

}

void MaterialBase::setLightPicker(LightPickerBase* value)
{
	if (m_lightPicker == value)
		return;

	m_lightPicker = value;
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->setLightPicker(m_lightPicker);
}

void MaterialBase::setMipmap(bool value)
{
	if (m_mipmap == value)
		return;

	m_mipmap = value;
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->setMipmap(value);

	m_depthPass->setMipmap(value);
	m_distancePass->setMipmap(value);
}

void MaterialBase::setSmooth(bool value)
{
	if (m_smooth == value)
		return;

	m_smooth = value;
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->setSmooth(value);

	m_depthPass->setSmooth(value);
	m_distancePass->setSmooth(value);
}

void MaterialBase::setRepeat(bool value)
{
	if (m_repeat == value)
		return;

	m_repeat = value;
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->setRepeat(value);

	m_depthPass->setRepeat(value);
	m_distancePass->setRepeat(value);
}

void MaterialBase::setBothSides(bool value)
{
	if (m_bothSides == value)
		return;

	m_bothSides = value;
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->setBothSides(m_bothSides);

	m_depthPass->setBothSides(m_bothSides);
	m_distancePass->setBothSides(m_bothSides);
}

void MaterialBase::setAlphaPremultiplied(bool value)
{
	if (m_alphaPremultiplied == value)
		return;

	m_alphaPremultiplied = value;
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->setAlphaPremultiplied(value);
}

bool MaterialBase::hasDepthAlphaThreshold()
{
	return m_depthPass->getAlphaThreshold() > 0;
}

void MaterialBase::activateForDepth(IContext* context, Camera3D* camera, bool distanceBased)
{
	m_distanceBasedDepthRender = distanceBased;

	if (distanceBased)
		m_distancePass->activate(context, camera);
	else
		m_depthPass->activate(context, camera);
}

void MaterialBase::deactivateForDepth(IContext* context)
{
	if (m_distanceBasedDepthRender)
		m_distancePass->deactivate(context);
	else
		m_depthPass->deactivate(context);
}

void MaterialBase::renderDepth(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	if (m_distanceBasedDepthRender)
	{
		if (renderable->getAnimator())
			m_distancePass->updateAnimationState(renderable, context, camera);

		m_distancePass->render(renderable, context, camera, viewProjection);
	}
	else
	{
		if (renderable->getAnimator())
			m_depthPass->updateAnimationState(renderable, context, camera);

		m_depthPass->render(renderable, context, camera, viewProjection);
	}
}

bool MaterialBase::passRendersToTexture(int index)
{
	return m_passes[index]->renderToTexture();
}

void MaterialBase::activatePass(int index, IContext* context, Camera3D* camera)
{
	m_passes[index]->activate(context, camera);
}

void MaterialBase::deactivatePass(int index, IContext* context)
{
	m_passes[index]->deactivate(context);
}

void MaterialBase::renderPass(int index, IRenderable* renderable, IContext* context, EntityCollector* entityCollector, Matrix3D& viewProjection)
{
	if (m_lightPicker)
		m_lightPicker->collectLights(renderable, entityCollector);

	MaterialPassBase* pass = m_passes[index];
	if (renderable->getAnimator())
		pass->updateAnimationState(renderable, context, entityCollector->getCamera());

	pass->render(renderable, context, entityCollector->getCamera(), viewProjection);
}

void MaterialBase::invalidatePasses(MaterialPassBase* triggerPass)
{
	m_depthPass->invalidateShaderProgram();
	m_distancePass->invalidateShaderProgram();

	// test if the depth and distance passes support animating the animation set in the vertex shader
	// if any object using this material fails to support accelerated animations for any of the passes,
	// we should do everything on cpu (otherwise we have the cost of both gpu + cpu animations)
	if (m_animationSet)
	{
		m_animationSet->resetGPUCompatibility();
		for (auto owner : m_owners)
		{
			IAnimator* animator = owner->getAnimator();
			if (animator)
			{
				animator->testGPUCompatibility(m_depthPass);
				animator->testGPUCompatibility(m_distancePass);
			}
		}
	}

	for (int i = 0; i < m_numPasses; i++)
	{
		// only invalidate the pass if it wasn't the triggering pass
		MaterialPassBase* pass = m_passes[i];
		if (pass != triggerPass)
			pass->invalidateShaderProgram(false);

		// test if animation will be able to run on gpu BEFORE compiling materials
		// test if the pass supports animating the animation set in the vertex shader
		// if any object using this material fails to support accelerated animations for any of the passes,
		// we should do everything on cpu (otherwise we have the cost of both gpu + cpu animations)
		if (m_animationSet)
		{
			for (auto owner : m_owners)
			{
				IAnimator* animator = owner->getAnimator();
				if (animator)
					animator->testGPUCompatibility(pass);
			}
		}
	}
}

void MaterialBase::addOwner(IMaterialOwner* owner)
{
	m_owners.push_back(owner);

	IAnimator* animator = owner->getAnimator();
	if (animator)
	{
		IAnimationSet* animationSet = animator->getAnimationSet();
		if (m_animationSet && animationSet != m_animationSet)
			assert(false); // Material instance cannot be shared across renderables with different animator libraries
		else
		{
			if (m_animationSet != animationSet)
			{
				m_animationSet = animationSet;
				for (int i = 0; i < m_numPasses; i++)
					m_passes[i]->setAnimationSet(m_animationSet);

				m_depthPass->setAnimationSet(m_animationSet);
				m_distancePass->setAnimationSet(m_animationSet);
				invalidatePasses(nullptr);
			}
		}
	}
}

void MaterialBase::removeOwner(IMaterialOwner* owner)
{
	auto iter = std::find(m_owners.begin(), m_owners.end(), owner);
	if (iter == m_owners.end())
		assert(false);
	else
		m_owners.erase(iter);

	if (m_owners.size() == 0)
	{
		m_animationSet = nullptr;
		for (int i = 0; i < m_numPasses; i++)
			m_passes[i]->setAnimationSet(m_animationSet);

		m_depthPass->setAnimationSet(m_animationSet);
		m_distancePass->setAnimationSet(m_animationSet);
		invalidatePasses(nullptr);
	}
}

void MaterialBase::updateMaterial(IContext* context)
{

}

void MaterialBase::deactivate(IContext* context)
{
	m_passes[m_numPasses - 1]->deactivate(context);
}

void MaterialBase::addPass(MaterialPassBase* pass)
{
	m_passes.push_back(pass);
	m_numPasses++;
	pass->setAnimationSet(m_animationSet);
	pass->setAlphaPremultiplied(m_alphaPremultiplied);
	pass->setMipmap(m_mipmap);
	pass->setSmooth(m_smooth);
	pass->setRepeat(m_repeat);
	pass->setLightPicker(m_lightPicker);
	pass->setBothSides(m_bothSides);
	pass->addEventListener(Event::CHANGE, CALLBACK_OBJECT(onPassChange));

	invalidatePasses(nullptr);
}

void MaterialBase::removePass(MaterialPassBase* pass)
{
	m_passes.erase(std::find(m_passes.begin(), m_passes.end(), pass));
	m_numPasses--;
}

void MaterialBase::clearPasses()
{
	for (int i = 0; i < m_numPasses; i++)
		m_passes[i]->removeEventListener(Event::CHANGE, CALLBACK_OBJECT(onPassChange));

	m_passes.clear();
	m_numPasses = 0;
}

void MaterialBase::onPassChange(Event* event)
{
	int mult = 1;
	m_renderOrderId = 0;

	for (int i = 0; i < m_numPasses; i++)
	{
		int id = m_passes[i]->m_programId;
		if (id != -1)
			m_renderOrderId += mult * id;

		mult *= 1000;
	}
}

void MaterialBase::onDepthPassChange(Event* event)
{
	int id = m_depthPass->m_programId;
	m_depthPassId = 0;

	if (id != -1)
		m_depthPassId += id;
}

void MaterialBase::onDistancePassChange(Event* event)
{
	int id = m_distancePass->m_programId;
	m_depthPassId = 0;

	if (id != -1)
		m_depthPassId += id;
}