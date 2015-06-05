#include "ShadowMapperBase.h"
#include "ShadowCasterCollector.h"
#include "EntityCollector.h"
#include "RenderTexture.h"

USING_AWAY_NAMESPACE

ShadowMapperBase::ShadowMapperBase()
{
	m_casterCollector = new ShadowCasterCollector();
	m_depthMap = nullptr;
	m_depthMapSize = 2048;
	m_explicitDepthMap = false;
	m_autoUpdateShadows = true;
	m_shadowsInvalid = false;
}

ShadowMapperBase::~ShadowMapperBase()
{

}

void ShadowMapperBase::setDepthMap(TextureProxyBase* depthMap)
{
	if (m_depthMap == depthMap)
		return;

	if (m_depthMap && !m_explicitDepthMap)
		delete m_depthMap;
	
	m_depthMap = depthMap;
	if (m_depthMap)
	{
		m_explicitDepthMap = true;
		m_depthMapSize = m_depthMap->getWidth();
	}
	else
	{
		m_explicitDepthMap = false;
	}
}

void ShadowMapperBase::setDepthMapSize(int value)
{
	if (value == m_depthMapSize)
		return;

	m_depthMapSize = value;
	if (m_explicitDepthMap)
	{
		// Cannot set depth map size for the current renderer
		assert(false);
	}
	else if (m_depthMap)
	{
		delete m_depthMap;
		m_depthMap = nullptr;
	}
}

TextureProxyBase* ShadowMapperBase::createDepthTexture()
{
	return new RenderTexture(m_depthMapSize, m_depthMapSize);
}

void ShadowMapperBase::renderDepthMap(IContext* context, EntityCollector* entityCollector, DepthRenderer* renderer)
{
	m_shadowsInvalid = false;
	updateDepthProjection(entityCollector->getCamera());
	if (!m_depthMap)
		m_depthMap = createDepthTexture();
	drawDepthMap(m_depthMap->getTextureForContext(context), entityCollector->m_scene, renderer);
}