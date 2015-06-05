#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A particle animation node that controls the rotation of a particle to always face the camera.
 */
class ParticleBillboardNode : public ParticleNodeBase
{
public:
	ParticleBillboardNode(Vector3D* billboardAxis = nullptr);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;
	void processAnimationSetting(ParticleAnimationSet* particleAnimationSet) override;

public:
	static const int MATRIX_INDEX;

	friend class ParticleBillboardState;
private:
	Vector3D* m_billboardAxis;
};

AWAY_NAMESPACE_END