#include "ParticleSegmentedScaleNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleSegmentedScaleState.h"

USING_AWAY_NAMESPACE

const int ParticleSegmentedScaleNode::START_INDEX = 0;

ParticleSegmentedScaleNode::ParticleSegmentedScaleNode(Vector3D& startScale, Vector3D& endScale, std::vector<Vector3D>& segmentScales) : ParticleNodeBase("ParticleSegmentedScale", ParticlePropertiesMode::GLOBAL, 0, 3)
{
	m_startScale = startScale;
	m_endScale = endScale;
	m_segmentScales = segmentScales;
}

void ParticleSegmentedScaleNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int accScale = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(accScale, 1);
	unsigned int tempScale = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(tempScale, 1);
	unsigned int temp = regCache->getFreeVertexVectorTemp();
	regCache->removeVertexTempUsage(tempScale);
	regCache->removeVertexTempUsage(accScale);

	unsigned int accTime = temp ^ Regs::x;
	unsigned int tempTime = temp ^ Regs::y;

	unsigned int startValue = regCache->getFreeVertexConstant();
	regCache->setRegisterIndex(this, START_INDEX, REGISTER_INDEX(startValue));

	std::vector<unsigned int> deltaValues;
	int numSegmentPoint = m_segmentScales.size();
	for (int i = 0; i <= numSegmentPoint; i++)
		deltaValues.push_back(regCache->getFreeVertexConstant());

	code.mov(accScale, startValue);

	for (int i = 0; i < numSegmentPoint; i++)
	{
		switch (i)
		{
		case 0:
			code.min(tempTime, regCache->m_vertexLife, deltaValues[i] ^ Regs::w);
			break;
		case 1:
			code.sub(accTime, regCache->m_vertexLife, deltaValues[i - 1] ^ Regs::w);
			code.max(tempTime, accTime, regCache->m_vertexZeroConst);
			code.min(tempTime, tempTime, deltaValues[i] ^ Regs::w);
			break;
		default:
			code.sub(accTime, accTime, deltaValues[i - 1] ^ Regs::w);
			code.max(tempTime, accTime, regCache->m_vertexZeroConst);
			code.min(tempTime, tempTime, deltaValues[i] ^ Regs::w);
			break;
		}

		code.mul(tempScale, tempTime, deltaValues[i]);
		code.add(accScale, accScale, tempScale);
	}

	// for the last segment
	if (numSegmentPoint == 0)
		tempTime = regCache->m_vertexLife;
	else
	{
		switch (numSegmentPoint)
		{
		case 1:
			code.sub(accTime, regCache->m_vertexLife, deltaValues[numSegmentPoint - 1] ^ Regs::w);
			break;
		default:
			code.sub(accTime, accTime, deltaValues[numSegmentPoint - 1] ^ Regs::w);
			break;
		}

		code.max(tempTime, accTime, regCache->m_vertexZeroConst);
	}

	code.mul(tempScale, tempTime, deltaValues[numSegmentPoint]);
	code.add(accScale, accScale, tempScale);
	code.mul(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, accScale);
}

AnimationStateBase* ParticleSegmentedScaleNode::createAnimationState(IAnimator* animator)
{
	return new ParticleSegmentedScaleState(animator, this);
}