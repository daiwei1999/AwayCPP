#include "LightProbe.h"
#include "NullBounds.h"
#include "LightProbeNode.h"

USING_AWAY_NAMESPACE

LightProbe::LightProbe(CubeTextureBase* diffuseMap, CubeTextureBase* specularMap)
{
	m_bounds = new NullBounds();
	m_worldBounds = new NullBounds();
	m_type = LightType::Probe;
	m_diffuseMap = diffuseMap;
	m_specularMap = specularMap;
}

void LightProbe::getObjectProjectionMatrix(IRenderable* renderable, Camera3D* camera, Matrix3D& result)
{
	assert(false); // Object projection matrices are not supported for LightProbe objects!
}

void LightProbe::updateBounds()
{
	m_boundsInvalid = false;
}

EntityNode* LightProbe::createEntityPartitionNode()
{
	return new LightProbeNode(this);
}