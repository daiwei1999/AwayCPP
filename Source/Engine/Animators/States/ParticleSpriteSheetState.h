#pragma once

#include "Common.h"
#include "ParticleStateBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleSpriteSheetState : public ParticleStateBase
{
public:
	ParticleSpriteSheetState(IAnimator* animator, ParticleSpriteSheetNode* particleSpriteSheetNode);

	void setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera) override;

private:
	void updateSpriteSheetData();

private:
	float m_spriteSheetData[8];
};

AWAY_NAMESPACE_END