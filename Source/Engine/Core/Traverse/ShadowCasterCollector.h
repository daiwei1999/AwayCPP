#pragma once

#include "Common.h"
#include "EntityCollector.h"

AWAY_NAMESPACE_BEGIN

/**
 * The ShadowCasterCollector class is a traverser for scene partitions that collects all scene graph entities which cast shadow.
 */
class ShadowCasterCollector : public EntityCollector
{
public:
	void applyRenderable(IRenderable* renderable) override;
	void applyUnknownLight(LightBase* light) override;
	void applyDirectionalLight(DirectionalLight* light) override;
	void applyPointLight(PointLight* light) override;
	void applyLightProbe(LightProbe* light) override;
	void applySkyBox(IRenderable* renderable) override;
};

AWAY_NAMESPACE_END