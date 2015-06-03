#pragma once

#include "Common.h"
#include "AnimatorBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an interface for assigning particle-based animation data sets to mesh-based entity objects
 * and controlling the various available states of animation through an interactive playhead that can be
 * automatically updated or manually triggered.
 *
 * Requires that the containing geometry of the parent mesh is particle geometry
 */
class ParticleAnimator : public AnimatorBase
{
public:
	ParticleAnimator(ParticleAnimationSet* particleAnimationSet);
	~ParticleAnimator();

	void start() override;
	void resetTime(int offset = 0);
	void setRenderState(IContext* context, IRenderable* renderable, int vertexConstantOffset, int vertexStreamOffset, Camera3D* camera) override;
	void testGPUCompatibility(MaterialPassBase* pass) override;

protected:
	void updateDeltaTime(float dt) override;

private:
	void generateAnimatorSubGeometry(SubMesh* subMesh);

private:
	int m_totalLenOfOneVertex;
	ParticleAnimationSet* m_particleAnimationSet;
	std::vector<ParticleStateBase*> m_animationParticleStates;
	std::vector<ParticleStateBase*> m_animatorParticleStates;
	std::vector<ParticleStateBase*> m_timeParticleStates;
	std::unordered_map<ISubGeometry*, AnimationSubGeometry*> m_animatorSubGeometries;
};

AWAY_NAMESPACE_END