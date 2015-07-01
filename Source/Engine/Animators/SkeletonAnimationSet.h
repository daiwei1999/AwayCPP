#pragma once

#include "Common.h"
#include "AnimationSetBase.h"

AWAY_NAMESPACE_BEGIN

class SkeletonAnimationSet : public AnimationSetBase
{
public:
	SkeletonAnimationSet(unsigned char jointsPerVertex = 4);

	unsigned char getJointsPerVertex() { return m_jointsPerVertex; }
	void getAGALVertexCode(ShaderChunk& code, MaterialPassBase* pass, std::vector<unsigned int>& sourceRegisters, std::vector<unsigned int>& targetRegisters, unsigned int uvSource, unsigned int uvTarget) override;
	void getAGALFragmentCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int shadedTarget) override;
	void getAGALUVCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int uvSource, unsigned int uvTarget) override;
	void doneAGALCode(MaterialPassBase* pass) override;
	void activate(IContext* context, MaterialPassBase* pass) override;
	void deactivate(IContext* context, MaterialPassBase* pass) override;

private:
	unsigned char m_jointsPerVertex;
};

AWAY_NAMESPACE_END