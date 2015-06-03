#include "ParticleScaleNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleScaleState.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

const int ParticleScaleNode::SCALE_INDEX = 0;
const std::string ParticleScaleNode::SCALE_VECTOR3D = "ScaleVector3D";

ParticleScaleNode::ParticleScaleNode(ParticlePropertiesMode mode, bool usesCycle, bool usesPhase, float minScale, float maxScale, float cycleDuration, float cyclePhase) : ParticleNodeBase("ParticleScale", mode, usesCycle && usesPhase ? 4 : (usesCycle || usesPhase ? 3 : 2), 3)
{
	m_usesCycle = usesCycle;
	m_usesPhase = usesPhase;
	m_minScale = minScale;
	m_maxScale = maxScale;
	m_cycleDuration = cycleDuration;
	m_cyclePhase = cyclePhase;
}

void ParticleScaleNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int scaleReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, SCALE_INDEX, REGISTER_INDEX(scaleReg));

	unsigned int temp = regCache->getFreeVertexSingleTemp();

	if (m_usesCycle)
	{
		code.mul(temp, regCache->m_vertexTime, scaleReg ^ Regs::z);

		if (m_usesPhase)
			code.add(temp, temp, scaleReg ^ Regs::w);

		code.sin(temp, temp);
	}

	code.mul(temp, scaleReg ^ Regs::y, m_usesCycle ? temp : regCache->m_vertexLife);
	code.add(temp, scaleReg ^ Regs::x, temp);
	code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp);
}

AnimationStateBase* ParticleScaleNode::createAnimationState(IAnimator* animator)
{
	return new ParticleScaleState(animator, this);
}

void ParticleScaleNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& scale = param[SCALE_VECTOR3D];
	if (m_usesCycle)
	{
		m_oneData[0] = (scale.m_x + scale.m_y) / 2;
		m_oneData[1] = std::abs(scale.m_x - scale.m_y) / 2;
		m_oneData[2] = MathConsts::TWO_PI / scale.m_z;

		if (m_usesPhase)
			m_oneData[3] = scale.m_w * MathConsts::DEGREES_TO_RADIANS;
	}
	else
	{
		m_oneData[0] = scale.m_x;
		m_oneData[1] = scale.m_y - scale.m_x;
	}
}