#include "ParticleTimeNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleTimeState.h"

USING_AWAY_NAMESPACE

const int ParticleTimeNode::TIME_STREAM_INDEX = 0;
const int ParticleTimeNode::TIME_CONSTANT_INDEX = 1;

ParticleTimeNode::ParticleTimeNode(bool usesDuration, bool usesLooping, bool usesDelay) : ParticleNodeBase("ParticleTime", ParticlePropertiesMode::LOCAL_STATIC, 4, 0)
{
	m_usesDuration = usesDuration;
	m_usesLooping = usesLooping;
	m_usesDelay = usesDelay;
}

void ParticleTimeNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int timeStreamReg = regCache->getFreeVertexAttribute(); // [startTime, duration, delay + duration, 1 / duration]
	regCache->setRegisterIndex(this, TIME_STREAM_INDEX, REGISTER_INDEX(timeStreamReg));
	unsigned int timeConstReg = regCache->getFreeVertexConstant();
	regCache->setRegisterIndex(this, TIME_CONSTANT_INDEX, REGISTER_INDEX(timeConstReg));

	unsigned int temp = regCache->getFreeVertexSingleTemp();
	code.sub(regCache->m_vertexTime, timeConstReg, timeStreamReg ^ Regs::x); // lifeTime = currentTime - startTime
	code.sge(temp, regCache->m_vertexTime, regCache->m_vertexZeroConst); // aliveFlag = (currentTime >= startTime) ? 1 : 0
	code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp);

	if (m_usesDuration)
	{
		if (m_usesLooping)
		{
			unsigned int div = regCache->getFreeVertexSingleTemp();
			if (m_usesDelay)
			{
				code.div(div, regCache->m_vertexTime, timeStreamReg ^ Regs::z);
				code.frc(div, div); // lifeRatio = fract((currentTime - startTime) / (delay + duration))
				code.mul(regCache->m_vertexTime, div, timeStreamReg ^ Regs::z); // lifeTime = lifeRatio * (delay + duration)
				code.slt(div, regCache->m_vertexTime, timeStreamReg ^ Regs::y); // aliveFlag = (lifeTime < duration) ? 1 : 0
				code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, div);
			}
			else
			{
				code.mul(div, regCache->m_vertexTime, timeStreamReg ^ Regs::w);
				code.frc(div, div); // lifeRatio = fract((currentTime - startTime) / duration)
				code.mul(regCache->m_vertexTime, div, timeStreamReg ^ Regs::y); // lifeTime = lifeRatio * duration
			}
		}
		else
		{
			unsigned int sge = regCache->getFreeVertexSingleTemp();
			code.sge(sge, timeStreamReg ^ Regs::y, regCache->m_vertexTime); // aliveFlag = (duration >= currentTime - startTime) ? 1 : 0
			code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, sge);
		}
	}

	code.mul(regCache->m_vertexLife, regCache->m_vertexTime, timeStreamReg ^ Regs::w); // lifePercent = lifeTime / duration
}

AnimationStateBase* ParticleTimeNode::createAnimationState(IAnimator* animator)
{
	return new ParticleTimeState(animator, this);
}

void ParticleTimeNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	m_oneData[0] = param.m_startTime;
	m_oneData[1] = param.m_duration;
	m_oneData[2] = param.m_delay + param.m_duration;
	m_oneData[3] = 1 / param.m_duration;
}