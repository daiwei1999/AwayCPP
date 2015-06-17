#include "ParticleNodeBase.h"

USING_AWAY_NAMESPACE

ParticleNodeBase::ParticleNodeBase(const std::string& name, ParticlePropertiesMode mode, int dataLength, int priority) : m_oneData(dataLength)
{
	std::string modeNames[3] = { "Global", "LocalStatic", "LocalDynamic" };
	setName(name + modeNames[static_cast<int>(mode)]);
	m_mode = mode;
	m_dataLength = dataLength;
	m_priority = priority;
}

void ParticleNodeBase::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{

}

void ParticleNodeBase::getAGALFragmentCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{

}

void ParticleNodeBase::getAGALUVCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{

}

void ParticleNodeBase::generatePropertyOfOneParticle(ParticleProperties& param)
{

}

void ParticleNodeBase::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{

}