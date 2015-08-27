#pragma once

#include "Common.h"
#include "AnimatorBase.h"
#include "SkeletonPose.h"

AWAY_NAMESPACE_BEGIN

class SubGeomAnimationState
{
public:
	SubGeomAnimationState(SkinnedSubGeometry* subGeom);
	~SubGeomAnimationState();

public:
	float* m_animatedVertexData;
	bool m_dirty = true;
};

/**
 * Provides an interface for assigning skeleton-based animation data sets to mesh-based entity objects
 * and controlling the various available states of animation through an interative playhead that can be
 * automatically updated or manually triggered.
 */
class SkeletonAnimator : public AnimatorBase
{
public:
	SkeletonAnimator(SkeletonAnimationSet* animationSet, Skeleton* skeleton, bool forceCPU = false);
	~SkeletonAnimator();

	std::vector<Matrix3D>& getGlobalMatrices();
	void play(const std::string& name, float offset = 0);
	void setRenderState(IContext* context, IRenderable* renderable, int vertexConstantOffset, int vertexStreamOffset, Camera3D* camera) override;
	void testGPUCompatibility(MaterialPassBase* pass) override;
	SkeletonAnimator* clone() override;

protected:
	void updateDeltaTime(float dt) override;

private:
	void updateGlobalProperties();
	void localToGlobalPose(SkeletonPose& sourcePose);
	void morphGeometry(float* targetData, SkinnedSubGeometry* subGeom);

private:
	int m_numTotalJoints;
	Skeleton* m_skeleton;
	bool m_forceCPU;
	bool m_globalPropertiesDirty;
	unsigned char m_jointsPerVertex;
	std::vector<float> m_matBones;
	std::vector<Matrix3D> m_globalMatrices;
	std::unordered_map<SkinnedSubGeometry*, SubGeomAnimationState*> m_subGeomAnimStates;
};

AWAY_NAMESPACE_END