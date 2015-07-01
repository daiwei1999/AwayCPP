#pragma once

#include "Common.h"
#include "AnimationSetBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * The animation data set used by particle-based animators, containing particle animation data.
 */
class ParticleAnimationSet : public AnimationSetBase
{
public:
	ParticleAnimationSet(bool usesDuration = false, bool usesLooping = false, bool usesDelay = false, bool usesRibbon = false);

	void generateAnimationSubGeometries(Mesh* mesh);
	std::vector<ParticleNodeBase*>& getParticleNodes() { return m_particleNodes; }

	void addAnimation(AnimationNodeBase* node) override;
	void getAGALVertexCode(ShaderChunk& code, MaterialPassBase* pass, std::vector<unsigned int>& sourceRegisters, std::vector<unsigned int>& targetRegisters, unsigned int uvSource, unsigned int uvTarget) override;
	void getAGALFragmentCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int shadedTarget) override;
	void getAGALUVCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int uvSource, unsigned int uvTarget) override;
	void doneAGALCode(MaterialPassBase* pass) override;
	void activate(IContext* context, MaterialPassBase* pass) override;
	void deactivate(IContext* context, MaterialPassBase* pass) override;

public:
	static const int POST_PRIORITY;
	static const int COLOR_PRIORITY;

	bool m_hasUVNode; // if has an node which will change UV
	bool m_needVelocity; // if the other nodes need to access the velocity
	bool m_hasBillboard; // if has a billboard node
	bool m_hasColorMulNode; // if has an node which will apply color multiple operation
	bool m_hasColorAddNode; // if has an node which will apply color add operation
	AnimationRegisterCache* m_animationRegisterCache;
	std::function<void(ParticleProperties&)> m_initParticleFunc;

private:
	int m_totalLenOfOneVertex;
	std::vector<ParticleNodeBase*> m_particleNodes;
	std::vector<ParticleNodeBase*> m_localStaticNodes;
	std::vector<ParticleNodeBase*> m_localDynamicNodes;
	std::unordered_map<ISubGeometry*, AnimationSubGeometry*> m_animationSubGeometries;
};

AWAY_NAMESPACE_END