#include "ParticleSegmentedColorNode.h"
#include "ParticleAnimationSet.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleSegmentedColorState.h"

USING_AWAY_NAMESPACE

const int ParticleSegmentedColorNode::START_MULTIPLIER_INDEX = 0;
const int ParticleSegmentedColorNode::START_OFFSET_INDEX = 1;
const int ParticleSegmentedColorNode::TIME_DATA_INDEX = 2;

ParticleSegmentedColorNode::ParticleSegmentedColorNode(bool usesMultiplier, bool usesOffset, ColorTransform startColor, ColorTransform endColor, std::vector<ColorSegmentPoint> segmentPoints) :
													   ParticleNodeBase("ParticleSegmentedColor", ParticlePropertiesMode::GLOBAL, 0, ParticleAnimationSet::COLOR_PRIORITY)
{
	m_usesMultiplier = usesMultiplier;
	m_usesOffset = usesOffset;
	m_startColor = startColor;
	m_endColor = endColor;
	m_segmentPoints = segmentPoints;
}

void ParticleSegmentedColorNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	if (!regCache->m_needFragmentAnimation)
		return;

	unsigned int accMultiplierColor = 0;
	if (m_usesMultiplier)
	{
		accMultiplierColor = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(accMultiplierColor, 1);
	}

	unsigned int tempColor = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(tempColor, 1);
	unsigned int temp = regCache->getFreeVertexVectorTemp();
	unsigned int accTime = temp ^ Regs::x;
	unsigned int tempTime = temp ^ Regs::y;

	regCache->removeVertexTempUsage(tempColor);
	if (m_usesMultiplier)
		regCache->removeVertexTempUsage(accMultiplierColor);

	// for saving all the life values (at most 4)
	unsigned int lifeTimeReg = regCache->getFreeVertexConstant();
	regCache->setRegisterIndex(this, TIME_DATA_INDEX, REGISTER_INDEX(lifeTimeReg));

	unsigned int startMulValue = 0;
	int numSegmentPoint = m_segmentPoints.size();
	std::vector<unsigned int> deltaMulValues;
	if (m_usesMultiplier)
	{
		startMulValue = regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, START_MULTIPLIER_INDEX, REGISTER_INDEX(startMulValue));
		for (int i = 0; i <= numSegmentPoint; i++)
			deltaMulValues.push_back(regCache->getFreeVertexConstant());
	}

	unsigned int startOffsetValue = 0;
	std::vector<unsigned int> deltaOffsetValues;
	if (m_usesOffset)
	{
		startOffsetValue = regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, START_OFFSET_INDEX, REGISTER_INDEX(startOffsetValue));
		for (int i = 0; i <= numSegmentPoint; i++)
			deltaOffsetValues.push_back(regCache->getFreeVertexConstant());
	}

	if (m_usesMultiplier)
		code.mov(accMultiplierColor, startMulValue);

	if (m_usesOffset)
		code.add(regCache->m_colorAddTarget, regCache->m_colorAddTarget, startOffsetValue);

	for (int i = 0; i < numSegmentPoint; i++)
	{
		switch (i)
		{
		case 0:
			code.min(tempTime, regCache->m_vertexLife, lifeTimeReg ^ Regs::x);
			break;
		case 1:
			code.sub(accTime, regCache->m_vertexLife, lifeTimeReg ^ Regs::x);
			code.max(tempTime, accTime, regCache->m_vertexZeroConst);
			code.min(tempTime, tempTime, lifeTimeReg ^ Regs::y);
			break;
		case 2:
			code.sub(accTime, accTime, lifeTimeReg ^ Regs::y);
			code.max(tempTime, accTime, regCache->m_vertexZeroConst);
			code.min(tempTime, tempTime, lifeTimeReg ^ Regs::z);
			break;
		case 3:
			code.sub(accTime, accTime, lifeTimeReg ^ Regs::z);
			code.max(tempTime, accTime, regCache->m_vertexZeroConst);
			code.min(tempTime, tempTime, lifeTimeReg ^ Regs::w);
			break;
		}

		if (m_usesMultiplier)
		{
			code.mul(tempColor, tempTime, deltaMulValues[i]);
			code.add(accMultiplierColor, accMultiplierColor, tempColor);
		}

		if (m_usesOffset)
		{
			code.mul(tempColor, tempTime, deltaOffsetValues[i]);
			code.add(regCache->m_colorAddTarget, regCache->m_colorAddTarget, tempColor);
		}
	}

	// for the last segment
	if (numSegmentPoint == 0)
		tempTime = regCache->m_vertexLife;
	else
	{
		switch (numSegmentPoint)
		{
		case 1:
			code.sub(accTime, regCache->m_vertexLife, lifeTimeReg ^ Regs::x);
			break;
		case 2:
			code.sub(accTime, accTime, lifeTimeReg ^ Regs::y);
			break;
		case 3:
			code.sub(accTime, accTime, lifeTimeReg ^ Regs::z);
			break;
		case 4:
			code.sub(accTime, accTime, lifeTimeReg ^ Regs::w);
			break;
		}
		code.max(tempTime, accTime, regCache->m_vertexZeroConst);
	}

	if (m_usesMultiplier)
	{
		code.mul(tempColor, tempTime, deltaMulValues[numSegmentPoint]);
		code.add(accMultiplierColor, accMultiplierColor, tempColor);
		code.mul(regCache->m_colorMulTarget, regCache->m_colorMulTarget, accMultiplierColor);
	}

	if (m_usesOffset)
	{
		code.mul(tempColor, tempTime, deltaOffsetValues[numSegmentPoint]);
		code.add(regCache->m_colorAddTarget, regCache->m_colorAddTarget, tempColor);
	}
}

AnimationStateBase* ParticleSegmentedColorNode::createAnimationState(IAnimator* animator)
{
	return new ParticleSegmentedColorState(animator, this);
}

void ParticleSegmentedColorNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	if (m_usesMultiplier)
		particleAnimationSet->m_hasColorMulNode = true;

	if (m_usesOffset)
		particleAnimationSet->m_hasColorAddNode = true;
}