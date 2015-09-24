#include "DefaultRenderer.h"
#include "DepthRenderer.h"
#include "EntityCollector.h"
#include "ShadowMapperBase.h"
#include "IContext.h"
#include "MaterialBase.h"
#include "IRenderable.h"
#include "Camera3D.h"
#include "DirectionalLight.h"
#include "PointLight.h"

USING_AWAY_NAMESPACE

#define RTT_PASSES 1
#define SCREEN_PASSES 2
#define ALL_PASSES 3

DefaultRenderer::DefaultRenderer()
{
	m_activeMaterial = nullptr;
	m_depthRenderer = new DepthRenderer();
	m_distanceRenderer = new DepthRenderer(false, true);
}

void DefaultRenderer::setContext(IContext* value)
{
	RendererBase::setContext(value);
	m_depthRenderer->setContext(value);
	m_distanceRenderer->setContext(value);
}

void DefaultRenderer::executeRender(EntityCollector* entityCollector, TextureBase* target, Rectangle<int>* scissorRect, int surfaceSelector)
{
	updateLights(entityCollector);

	// otherwise RTT will interfere with other RTTs
	if (target)
	{
		drawRenderables(entityCollector->getOpaqueRenderableHead(), entityCollector, RTT_PASSES);
		drawRenderables(entityCollector->getBlendedRenderableHead(), entityCollector, RTT_PASSES);
	}

	RendererBase::executeRender(entityCollector, target, scissorRect, surfaceSelector);
}

void DefaultRenderer::draw(EntityCollector* entityCollector, TextureBase* target)
{
	m_context->setBlendFactors(BlendFactor::ONE, BlendFactor::ZERO);

	if (entityCollector->getSkyBox())
	{
		if (m_activeMaterial)
		{
			m_activeMaterial->deactivate(m_context);
			m_activeMaterial = nullptr;
		}
		m_context->setDepthTest(false, CompareMode::ALWAYS);
		drawSkyBox(entityCollector);
	}

	m_context->setDepthTest(true, CompareMode::LESS_EQUAL);

	int which = target ? SCREEN_PASSES : ALL_PASSES;
	drawRenderables(entityCollector->getOpaqueRenderableHead(), entityCollector, which);
	drawRenderables(entityCollector->getBlendedRenderableHead(), entityCollector, which);

	m_context->setDepthTest(false, CompareMode::LESS_EQUAL);

	if (m_activeMaterial)
	{
		m_activeMaterial->deactivate(m_context);
		m_activeMaterial = nullptr;
	}
}

void DefaultRenderer::updateLights(EntityCollector* entityCollector)
{
	DirectionalLightVector& dirLights = entityCollector->getDirectionalLights();
	PointLightVector& pointLights = entityCollector->getPointLights();

	for (auto dirLight : dirLights)
	{
		ShadowMapperBase* shadowMapper = dirLight->getShadowMapper();
		if (dirLight->getCastsShadows() && (shadowMapper->getAutoUpdateShadows() || shadowMapper->isShadowsInvalid()))
			shadowMapper->renderDepthMap(m_context, entityCollector, m_depthRenderer);
	}

	for (auto pointLight : pointLights)
	{
		ShadowMapperBase* shadowMapper = pointLight->getShadowMapper();
		if (pointLight->getCastsShadows() && (shadowMapper->getAutoUpdateShadows() || shadowMapper->isShadowsInvalid()))
			shadowMapper->renderDepthMap(m_context, entityCollector, m_distanceRenderer);
	}
}

void DefaultRenderer::drawSkyBox(EntityCollector* entityCollector)
{
	IRenderable* skyBox = entityCollector->getSkyBox();
	MaterialBase* material = skyBox->getMaterial();
	Camera3D* camera = entityCollector->getCamera();

	updateSkyBoxProjection(camera);

	material->activatePass(0, m_context, camera);
	material->renderPass(0, skyBox, m_context, entityCollector, m_skyboxProjection);
	material->deactivatePass(0, m_context);
}

void DefaultRenderer::updateSkyBoxProjection(Camera3D* camera)
{
	Vector3D tempVector;
	m_skyboxProjection.copyFrom(m_rttViewProjectionMatrix);
	m_skyboxProjection.copyRowTo(2, tempVector);
	Vector3D* cameraPos = camera->getScenePosition();
	float cx = tempVector.m_x;
	float cy = tempVector.m_y;
	float cz = tempVector.m_z;
	float length = std::sqrt(cx * cx + cy * cy + cz * cz);
	tempVector.m_x = 0;
	tempVector.m_y = 0;
	tempVector.m_z = 0;
	tempVector.m_w = 1;

	Matrix3D tempMatrix;
	tempMatrix.copyFrom(camera->getSceneTransform());
	tempMatrix.copyColumnFrom(3, tempVector);
	tempVector.m_x = 0;
	tempVector.m_y = 0;
	tempVector.m_z = 1;
	tempVector.m_w = 0;

	Matrix3D::transformVector(tempMatrix, &tempVector, &tempVector);
	tempVector.normalize();

	float angle = std::acos(tempVector.m_x * (cx / length) + tempVector.m_y * (cy / length) + tempVector.m_z * (cz / length));
	if (std::abs(angle) > 0.000001f)
		return;

	float cw = -(cx * cameraPos->m_x + cy * cameraPos->m_y + cz * cameraPos->m_z + length);
	float signX = (cx >= 0 ? 1.f : -1.f);
	float signY = (cy >= 0 ? 1.f : -1.f);

	tempVector.m_x = signX;
	tempVector.m_y = signY;
	tempVector.m_z = 1;
	tempVector.m_w = 1;
	tempMatrix.copyFrom(m_skyboxProjection);
	tempMatrix.invert();

	Vector3D q;
	Matrix3D::transformVector(tempMatrix, &tempVector, &q);
	m_skyboxProjection.copyRowTo(3, tempVector);
	float a = (q.m_x * tempVector.m_x + q.m_y * tempVector.m_y + q.m_z * tempVector.m_z + q.m_w * tempVector.m_w) / (cx * q.m_x + cy * q.m_y + cz * q.m_z + cw * q.m_w);
	tempVector.m_x = cx * a;
	tempVector.m_y = cy * a;
	tempVector.m_z = cz * a;
	tempVector.m_w = cw * a;
	m_skyboxProjection.copyRowFrom(2, tempVector);
}

void DefaultRenderer::drawRenderables(RenderableListItem* item, EntityCollector* entityCollector, int which)
{
	RenderableListItem* item2;
	Camera3D* camera = entityCollector->getCamera();
	while (item)
	{
		m_activeMaterial = item->m_renderable->getMaterial();
		m_activeMaterial->updateMaterial(m_context);

		int i = 0, numPasses = m_activeMaterial->getPassCount();
		do
		{
			item2 = item;
			int rttMask = m_activeMaterial->passRendersToTexture(i) ? RTT_PASSES : SCREEN_PASSES;
			if ((rttMask & which) != 0)
			{
				m_activeMaterial->activatePass(i, m_context, camera);
				do 
				{
					m_activeMaterial->renderPass(i, item2->m_renderable, m_context, entityCollector, m_rttViewProjectionMatrix);
					item2 = item2->m_next;
				} while (item2 && item2->m_renderable->getMaterial() == m_activeMaterial);
				m_activeMaterial->deactivatePass(i, m_context);
			}
			else
			{
				do 
				{
					item2 = item2->m_next;
				} while (item2 && item2->m_renderable->getMaterial() == m_activeMaterial);
			}
		} while (++i < numPasses);

		item = item2;
	}
}