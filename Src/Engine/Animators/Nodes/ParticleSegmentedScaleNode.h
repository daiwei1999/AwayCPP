#pragma once

#include "Common.h"
#include "ParticleNodeBase.h"

AWAY_NAMESPACE_BEGIN

class ParticleSegmentedScaleNode : public ParticleNodeBase
{
public:
	ParticleSegmentedScaleNode(Vector3D& startScale, Vector3D& endScale, std::vector<Vector3D>& segmentScales);

	void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache) override;
	AnimationStateBase* createAnimationState(IAnimator* animator) override;

public:
	static const int START_INDEX;

	friend class ParticleSegmentedScaleState;
private:
	Vector3D m_startScale;
	Vector3D m_endScale;
	std::vector<Vector3D> m_segmentScales;
};

AWAY_NAMESPACE_END