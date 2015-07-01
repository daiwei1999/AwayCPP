#include "ParticleTimeNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleTimeState.h"

USING_AWAY_NAMESPACE

const int ParticleTimeNode::TIME_STREAM_INDEX = 0;
const int ParticleTimeNode::TIME_CONSTANT_INDEX = 1;

ParticleTimeNode::ParticleTimeNode(bool usesDuration, bool usesLooping, bool usesDelay, bool usesRibbon) : ParticleNodeBase("ParticleTime", ParticlePropertiesMode::LOCAL_STATIC, 4, 0)
{
	m_usesDuration = usesDuration;
	m_usesLooping = usesLooping;
	m_usesDelay = usesDelay;
	m_usesRibbon = usesRibbon;
}

void ParticleTimeNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int timeStreamReg = regCache->getFreeVertexAttribute(); // [startTime, duration, delay + duration, m_usesRibbon ? lagTime : 1 / duration]
	regCache->setRegisterIndex(this, TIME_STREAM_INDEX, REGISTER_INDEX(timeStreamReg));
	unsigned int timeConstReg = regCache->getFreeVertexConstant(); // [currentTime, 0, 0, 0]
	regCache->setRegisterIndex(this, TIME_CONSTANT_INDEX, REGISTER_INDEX(timeConstReg));

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	code.sub(regCache->m_vertexTime, timeConstReg ^ Regs::x, timeStreamReg ^ Regs::x); // vertexTime = currentTime - startTime

	if (m_usesRibbon)
	{
		code.mul(temp ^ Regs::x, regCache->m_uvAttribute ^ Regs::y, timeStreamReg ^ Regs::w); // temp.x = v * lagTime
		code.sub(regCache->m_vertexTime, regCache->m_vertexTime, temp ^ Regs::x);
		code.max(regCache->m_vertexTime, regCache->m_vertexTime, regCache->m_vertexZeroConst);
	}
	else
	{
		code.sge(temp ^ Regs::x, regCache->m_vertexTime, regCache->m_vertexZeroConst); // aliveFlag = (vertexTime >= 0) ? 1 : 0
		code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp ^ Regs::x);
	}

	if (m_usesDuration)
	{
		if (m_usesLooping)
		{
			if (m_usesDelay)
			{
				if (m_usesRibbon)
				{
					code.add(temp ^ Regs::y, timeStreamReg ^ Regs::z, timeStreamReg ^ Regs::w); // temp.y = duration + lagTime + delay
					code.div(temp ^ Regs::z, regCache->m_vertexTime, temp ^ Regs::y); // temp.z = vertexTime / temp.y
					code.frc(temp ^ Regs::z, temp ^ Regs::z);
					code.mul(regCache->m_vertexTime, temp ^ Regs::z, temp ^ Regs::y); // vertexTime = fract(vertexTime / temp.y) * temp.y

					// if ribbon finished shrink, reset ribbon to initial state
					code.add(temp ^ Regs::z, regCache->m_vertexTime, temp ^ Regs::x); // temp.z = vertexTime + temp.x
					code.add(temp ^ Regs::y, timeStreamReg ^ Regs::y, timeStreamReg ^ Regs::w); // temp.y = duration + lagTime
					code.slt(temp ^ Regs::z, temp ^ Regs::z, temp ^ Regs::y); // if temp.z >= duration + lagTime, set vertexTime to 0
					code.mul(regCache->m_vertexTime, regCache->m_vertexTime, temp ^ Regs::z);

					// let ribbon shrink into a line
					code.min(regCache->m_vertexTime, regCache->m_vertexTime, timeStreamReg ^ Regs::y);
				}
				else
				{
					code.div(temp ^ Regs::x, regCache->m_vertexTime, timeStreamReg ^ Regs::z);
					code.frc(temp ^ Regs::x, temp ^ Regs::x); // temp.x = fract(vertexTime / (delay + duration))
					code.mul(regCache->m_vertexTime, temp ^ Regs::x, timeStreamReg ^ Regs::z); // vertexTime = temp.x * (delay + duration)
					code.slt(temp ^ Regs::x, regCache->m_vertexTime, timeStreamReg ^ Regs::y); // aliveFlag = (vertexTime < duration) ? 1 : 0
					code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp ^ Regs::x);
				}
			}
			else
			{
				if (m_usesRibbon)
				{
					code.add(temp ^ Regs::y, timeStreamReg ^ Regs::y, timeStreamReg ^ Regs::w); // temp.y = duration + lagTime
					code.div(temp ^ Regs::z, regCache->m_vertexTime, temp ^ Regs::y);
					code.frc(temp ^ Regs::z, temp ^ Regs::z);
					code.mul(regCache->m_vertexTime, temp ^ Regs::z, temp ^ Regs::y); // vertexTime = fract(vertexTime / (duration + lagTime)) * (duration + lagTime)

					// if ribbon finished shrink, reset ribbon to initial state
					code.add(temp ^ Regs::z, regCache->m_vertexTime, temp ^ Regs::x); // temp.z = vertexTime + temp.x
					code.slt(temp ^ Regs::z, temp ^ Regs::z, temp ^ Regs::y); // if temp.z >= duration + lagTime, set vertexTime to 0
					code.mul(regCache->m_vertexTime, regCache->m_vertexTime, temp ^ Regs::z);

					// let ribbon shrink into a line
					code.min(regCache->m_vertexTime, regCache->m_vertexTime, timeStreamReg ^ Regs::y);
				}
				else
				{
					code.mul(temp ^ Regs::x, regCache->m_vertexTime, timeStreamReg ^ Regs::w);
					code.frc(temp ^ Regs::x, temp ^ Regs::x);
					code.mul(regCache->m_vertexTime, temp ^ Regs::x, timeStreamReg ^ Regs::y); // vertexTime = fract(vertexTime / duration) * duration
				}
			}
		}
		else
		{
			if (m_usesRibbon)
			{
				code.min(regCache->m_vertexTime, regCache->m_vertexTime, timeStreamReg ^ Regs::y);
			}
			else
			{
				code.sge(temp ^ Regs::x, timeStreamReg ^ Regs::y, regCache->m_vertexTime); // aliveFlag = (vertexTime <= duration) ? 1 : 0
				code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp ^ Regs::x);
			}
		}
	}

	// vertexLife = vertexTime / duration
	if (m_usesRibbon)
		code.div(regCache->m_vertexLife, regCache->m_vertexTime, timeStreamReg ^ Regs::y);
	else
		code.mul(regCache->m_vertexLife, regCache->m_vertexTime, timeStreamReg ^ Regs::w);
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
	m_oneData[3] = m_usesRibbon ? param.m_lagTime : 1 / param.m_duration;
}