#pragma once

#include "Common.h"
#include "AnimationNodeBase.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

enum class ParticlePropertiesMode
{
	GLOBAL,       // Mode that defines the particle node as acting on global properties (ie. the properties set in the node constructor or the corresponding animation state).
	LOCAL_STATIC, // Mode that defines the particle node as acting on local static properties (ie. the properties of particles set in the initializing function on the animation set).
	LOCAL_DYNAMIC // Mode that defines the particle node as acting on local dynamic properties (ie. the properties of the particles set in the corresponding animation state).
};

class ParticleProperties
{
public:
	int m_index; // The index of the current particle being set
	int m_total; // The total number of particles being processed by the particle animation set
	float m_startTime; // The start time of the particle
	float m_duration; // The duration of the particle, an optional value used when useDuration parameter is enabled in ParticleAnimationSet's constructor
	float m_delay; // The delay between cycles of the particle, an optional value used when useLooping and useDelay are enabled in ParticleAnimationSet's constructor
	float m_lagTime; // The shrink time of the particle, an optional value used when usesRibbon are enabled in ParticleAnimationSet's constructor
	Vector3D& operator[](const std::string& name) { return m_extra[name]; }

private:
	std::unordered_map<std::string, Vector3D> m_extra;
};

/**
 * Provides an abstract base class for particle animation nodes.
 */
class ParticleNodeBase : public AnimationNodeBase
{
public:
	ParticleNodeBase(const std::string& name, ParticlePropertiesMode mode, int dataLength, int priority = 1);

	ParticlePropertiesMode getMode() { return m_mode; }
	int getPriority() { return m_priority; }
	int getDataLength() { return m_dataLength; }
	float* getOneData() { return m_oneData.data(); }

	virtual void getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache);
	virtual void getAGALFragmentCode(ShaderChunk& code, AnimationRegisterCache* regCache);
	virtual void getAGALUVCode(ShaderChunk& code, AnimationRegisterCache* regCache);
	virtual void generatePropertyOfOneParticle(ParticleProperties& param);
	virtual void processAnimationSetting(ParticleAnimationSet* particleAnimationSet);

public:
	int m_dataOffset;

protected:
	int m_dataLength;
	ParticlePropertiesMode m_mode;
	std::vector<float> m_oneData;

private:
	int m_priority;
};

AWAY_NAMESPACE_END