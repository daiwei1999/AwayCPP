#include "ParticleOscillatorNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleOscillatorState.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

const int ParticleOscillatorNode::OSCILLATOR_INDEX = 0;
const std::string ParticleOscillatorNode::OSCILLATOR_VECTOR3D = "OscillatorVector3D";

ParticleOscillatorNode::ParticleOscillatorNode(ParticlePropertiesMode mode, Vector3D* oscillator) : ParticleNodeBase("ParticleOscillator", mode, 4)
{
	m_oscillator = oscillator;
}

void ParticleOscillatorNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int oscillatorReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, OSCILLATOR_INDEX, REGISTER_INDEX(oscillatorReg));

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(temp, 1);
	unsigned int temp2 = regCache->getFreeVertexVectorTemp();
	regCache->removeVertexTempUsage(temp);

	code.mul(temp ^ Regs::x, regCache->m_vertexTime, oscillatorReg ^ Regs::w);
	code.sin(temp ^ Regs::y, temp ^ Regs::x);
	code.mul(temp2 ^ Regs::xyz, temp ^ Regs::y, oscillatorReg ^ Regs::xyz);
	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget ^ Regs::xyz, temp2);

	if (regCache->m_needVelocity)
	{
		code.cos(temp ^ Regs::z, temp ^ Regs::x);
		code.mul(temp2 ^ Regs::xyz, temp ^ Regs::z, oscillatorReg ^ Regs::xyz);
		code.add(regCache->m_velocityTarget ^ Regs::xyz, regCache->m_velocityTarget, temp2);
	}
}

AnimationStateBase* ParticleOscillatorNode::createAnimationState(IAnimator* animator)
{
	return new ParticleOscillatorState(animator, this);
}

void ParticleOscillatorNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& drift = param[OSCILLATOR_VECTOR3D];
	m_oneData[0] = drift.m_x;
	m_oneData[1] = drift.m_y;
	m_oneData[2] = drift.m_z;
	m_oneData[3] = MathConsts::TWO_PI / drift.m_w;
}