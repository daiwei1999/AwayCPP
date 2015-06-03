#include "ParticleColorNode.h"
#include "ParticleAnimationSet.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleColorState.h"

USING_AWAY_NAMESPACE

const int ParticleColorNode::START_MULTIPLIER_INDEX = 0;
const int ParticleColorNode::DELTA_MULTIPLIER_INDEX = 1;
const int ParticleColorNode::START_OFFSET_INDEX = 2;
const int ParticleColorNode::DELTA_OFFSET_INDEX = 3;
const int ParticleColorNode::CYCLE_INDEX = 4;
const std::string ParticleColorNode::COLOR_START_MULTIPLIER = "ColorStartMultiplier";
const std::string ParticleColorNode::COLOR_START_OFFSET = "ColorStartOffset";
const std::string ParticleColorNode::COLOR_END_MULTIPLIER = "ColorEndMultiplier";
const std::string ParticleColorNode::COLOR_END_OFFSET = "ColorEndOffset";

ParticleColorNode::ParticleColorNode(ParticlePropertiesMode mode, bool usesMultiplier, bool usesOffset, bool usesCycle, bool usesPhase,
									 ColorTransform* startColor, ColorTransform* endColor, float cycleDuration, float cyclePhase) : ParticleNodeBase("ParticleColor", mode, (usesMultiplier && usesOffset) ? 16 : 8, ParticleAnimationSet::COLOR_PRIORITY)
{
	m_usesMultiplier = usesMultiplier;
	m_usesOffset = usesOffset;
	m_usesCycle = usesCycle;
	m_usesPhase = usesPhase;
	m_startColor = startColor;
	m_endColor = endColor;
	m_cycleDuration = cycleDuration;
	m_cyclePhase = cyclePhase;
}

void ParticleColorNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	if (!regCache->m_needFragmentAnimation)
		return;

	unsigned int sin = 0;
	unsigned int temp = regCache->getFreeVertexVectorTemp();

	if (m_usesCycle)
	{
		unsigned int cycleConst = regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, CYCLE_INDEX, REGISTER_INDEX(cycleConst));

		regCache->addVertexTempUsages(temp, 1);
		sin = regCache->getFreeVertexSingleTemp();
		regCache->removeVertexTempUsage(temp);

		code.mul(sin, regCache->m_vertexTime, cycleConst ^ Regs::x);

		if (m_usesPhase)
			code.add(sin, sin, cycleConst ^ Regs::y);

		code.sin(sin, sin);
	}

	if (m_usesMultiplier)
	{
		unsigned int startMultiplierReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
		unsigned int deltaMultiplierReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
		regCache->setRegisterIndex(this, START_MULTIPLIER_INDEX, REGISTER_INDEX(startMultiplierReg));
		regCache->setRegisterIndex(this, DELTA_MULTIPLIER_INDEX, REGISTER_INDEX(deltaMultiplierReg));

		code.mul(temp, deltaMultiplierReg, m_usesCycle ? sin : regCache->m_vertexLife);
		code.add(temp, temp, startMultiplierReg);
		code.mul(regCache->m_colorMulTarget, regCache->m_colorMulTarget, temp);
	}

	if (m_usesOffset)
	{
		unsigned int startOffsetReg = (m_mode == ParticlePropertiesMode::LOCAL_STATIC) ? regCache->getFreeVertexAttribute() : regCache->getFreeVertexConstant();
		unsigned int deltaOffsetReg = (m_mode == ParticlePropertiesMode::LOCAL_STATIC) ? regCache->getFreeVertexAttribute() : regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, START_OFFSET_INDEX, REGISTER_INDEX(startOffsetReg));
		regCache->setRegisterIndex(this, DELTA_OFFSET_INDEX, REGISTER_INDEX(deltaOffsetReg));

		code.mul(temp, deltaOffsetReg, m_usesCycle ? sin : regCache->m_vertexLife);
		code.add(temp, temp, startOffsetReg);
		code.add(regCache->m_colorAddTarget, regCache->m_colorAddTarget, temp);
	}
}

AnimationStateBase* ParticleColorNode::createAnimationState(IAnimator* animator)
{
	return new ParticleColorState(animator, this);
}

void ParticleColorNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	if (m_usesMultiplier)
		particleAnimationSet->m_hasColorMulNode = true;

	if (m_usesOffset)
		particleAnimationSet->m_hasColorAddNode = true;
}

void ParticleColorNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	int offset = 0;
	Vector3D& startColorMultiplier = param[COLOR_START_MULTIPLIER];
	Vector3D& startColorOffset = param[COLOR_START_OFFSET];
	Vector3D& endColorMultiplier = param[COLOR_END_MULTIPLIER];
	Vector3D& endColorOffset = param[COLOR_END_OFFSET];

	if (m_usesCycle)
	{
		if (m_usesMultiplier)
		{
			m_oneData[0] = (startColorMultiplier.m_x + endColorMultiplier.m_x) / 2;
			m_oneData[1] = (startColorMultiplier.m_y + endColorMultiplier.m_y) / 2;
			m_oneData[2] = (startColorMultiplier.m_z + endColorMultiplier.m_z) / 2;
			m_oneData[3] = (startColorMultiplier.m_w + endColorMultiplier.m_w) / 2;
			m_oneData[4] = (startColorMultiplier.m_x - endColorMultiplier.m_x) / 2;
			m_oneData[5] = (startColorMultiplier.m_y - endColorMultiplier.m_y) / 2;
			m_oneData[6] = (startColorMultiplier.m_z - endColorMultiplier.m_z) / 2;
			m_oneData[7] = (startColorMultiplier.m_w - endColorMultiplier.m_w) / 2;
			offset = 8;
		}

		if (m_usesOffset)
		{
			m_oneData[offset] = (startColorOffset.m_x + endColorOffset.m_x) / 510;
			m_oneData[offset + 1] = (startColorOffset.m_y + endColorOffset.m_y) / 510;
			m_oneData[offset + 2] = (startColorOffset.m_z + endColorOffset.m_z) / 510;
			m_oneData[offset + 3] = (startColorOffset.m_w + endColorOffset.m_w) / 510;
			m_oneData[offset + 4] = (startColorOffset.m_x - endColorOffset.m_x) / 510;
			m_oneData[offset + 5] = (startColorOffset.m_y - endColorOffset.m_y) / 510;
			m_oneData[offset + 6] = (startColorOffset.m_z - endColorOffset.m_z) / 510;
			m_oneData[offset + 7] = (startColorOffset.m_w - endColorOffset.m_w) / 510;
		}
	}
	else
	{
		if (m_usesMultiplier)
		{
			m_oneData[0] = startColorMultiplier.m_x;
			m_oneData[1] = startColorMultiplier.m_y;
			m_oneData[2] = startColorMultiplier.m_z;
			m_oneData[3] = startColorMultiplier.m_w;
			m_oneData[4] = endColorMultiplier.m_x - startColorMultiplier.m_x;
			m_oneData[5] = endColorMultiplier.m_y - startColorMultiplier.m_y;
			m_oneData[6] = endColorMultiplier.m_z - startColorMultiplier.m_z;
			m_oneData[7] = endColorMultiplier.m_w - startColorMultiplier.m_w;
			offset = 8;
		}

		if (m_usesOffset)
		{
			m_oneData[offset] = startColorOffset.m_x / 255;
			m_oneData[offset + 1] = startColorOffset.m_y / 255;
			m_oneData[offset + 2] = startColorOffset.m_z / 255;
			m_oneData[offset + 3] = startColorOffset.m_w / 255;
			m_oneData[offset + 4] = (endColorOffset.m_x - startColorOffset.m_x) / 255;
			m_oneData[offset + 5] = (endColorOffset.m_y - startColorOffset.m_y) / 255;
			m_oneData[offset + 6] = (endColorOffset.m_z - startColorOffset.m_z) / 255;
			m_oneData[offset + 7] = (endColorOffset.m_w - startColorOffset.m_w) / 255;
		}
	}
}