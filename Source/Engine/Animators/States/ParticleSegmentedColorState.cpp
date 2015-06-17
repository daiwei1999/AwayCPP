#include "ParticleSegmentedColorState.h"
#include "ParticleSegmentedColorNode.h"
#include "AnimationRegisterCache.h"

USING_AWAY_NAMESPACE

ParticleSegmentedColorState::ParticleSegmentedColorState(IAnimator* animator, ParticleSegmentedColorNode* particleSegmentedColorNode) : ParticleStateBase(animator, particleSegmentedColorNode)
{
	updateColorData();
}

void ParticleSegmentedColorState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_needFragmentAnimation)
	{
		ParticleSegmentedColorNode* particleSegmentedColorNode = static_cast<ParticleSegmentedColorNode*>(m_animationNode);
		if (particleSegmentedColorNode->m_segmentPoints.size() > 0)
			regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleSegmentedColorNode::TIME_DATA_INDEX), m_timeLifeData[0], m_timeLifeData[1], m_timeLifeData[2], m_timeLifeData[3]);

		if (particleSegmentedColorNode->m_usesMultiplier)
			regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleSegmentedColorNode::START_MULTIPLIER_INDEX), m_multiplierData);

		if (particleSegmentedColorNode->m_usesOffset)
			regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleSegmentedColorNode::START_OFFSET_INDEX), m_offsetData);
	}
}

void ParticleSegmentedColorState::updateColorData()
{
	ParticleSegmentedColorNode* particleSegmentedColorNode = static_cast<ParticleSegmentedColorNode*>(m_animationNode);
	ColorTransform& startColor = particleSegmentedColorNode->m_startColor;
	ColorTransform& endColor = particleSegmentedColorNode->m_endColor;
	std::vector<ColorSegmentPoint>& segmentPoints = particleSegmentedColorNode->m_segmentPoints;

	int numSegmentPoint = segmentPoints.size();
	for (int i = 0; i < numSegmentPoint; i++)
	{
		if (i == 0)
			m_timeLifeData.push_back(segmentPoints[i].m_life);
		else
			m_timeLifeData.push_back(segmentPoints[i].m_life - segmentPoints[i - 1].m_life);
	}

	if (numSegmentPoint == 0)
		m_timeLifeData.push_back(1);
	else
		m_timeLifeData.push_back(1 - segmentPoints[numSegmentPoint - 1].m_life);

	if (particleSegmentedColorNode->m_usesMultiplier)
	{
		m_multiplierData.push_back(startColor.m_redMultiplier);
		m_multiplierData.push_back(startColor.m_greenMultiplier);
		m_multiplierData.push_back(startColor.m_blueMultiplier);
		m_multiplierData.push_back(startColor.m_alphaMultiplier);

		for (int i = 0; i < numSegmentPoint; i++)
		{
			float timeLife = m_timeLifeData[i];
			ColorTransform& currentColor = segmentPoints[i].m_color;
			if (i == 0)
			{
				m_multiplierData.push_back((currentColor.m_redMultiplier - startColor.m_redMultiplier) / timeLife);
				m_multiplierData.push_back((currentColor.m_greenMultiplier - startColor.m_greenMultiplier) / timeLife);
				m_multiplierData.push_back((currentColor.m_blueMultiplier - startColor.m_blueMultiplier) / timeLife);
				m_multiplierData.push_back((currentColor.m_alphaMultiplier - startColor.m_alphaMultiplier) / timeLife);
			}
			else
			{
				ColorTransform& previousColor = segmentPoints[i - 1].m_color;
				m_multiplierData.push_back((currentColor.m_redMultiplier - previousColor.m_redMultiplier) / timeLife);
				m_multiplierData.push_back((currentColor.m_greenMultiplier - previousColor.m_greenMultiplier) / timeLife);
				m_multiplierData.push_back((currentColor.m_blueMultiplier - previousColor.m_blueMultiplier) / timeLife);
				m_multiplierData.push_back((currentColor.m_alphaMultiplier - previousColor.m_alphaMultiplier) / timeLife);
			}
		}

		if (numSegmentPoint == 0)
		{
			m_multiplierData.push_back(endColor.m_redMultiplier - startColor.m_redMultiplier);
			m_multiplierData.push_back(endColor.m_greenMultiplier - startColor.m_greenMultiplier);
			m_multiplierData.push_back(endColor.m_blueMultiplier - startColor.m_blueMultiplier);
			m_multiplierData.push_back(endColor.m_alphaMultiplier - startColor.m_alphaMultiplier);
		}
		else
		{
			float timeLife = m_timeLifeData[numSegmentPoint];
			ColorTransform& previousColor = segmentPoints[numSegmentPoint - 1].m_color;
			m_multiplierData.push_back((endColor.m_redMultiplier - previousColor.m_redMultiplier) / timeLife);
			m_multiplierData.push_back((endColor.m_greenMultiplier - previousColor.m_greenMultiplier) / timeLife);
			m_multiplierData.push_back((endColor.m_blueMultiplier - previousColor.m_blueMultiplier) / timeLife);
			m_multiplierData.push_back((endColor.m_alphaMultiplier - previousColor.m_alphaMultiplier) / timeLife);
		}
	}

	if (particleSegmentedColorNode->m_usesOffset)
	{
		m_offsetData.push_back(startColor.m_redOffset / 255);
		m_offsetData.push_back(startColor.m_greenOffset / 255);
		m_offsetData.push_back(startColor.m_blueOffset / 255);
		m_offsetData.push_back(startColor.m_alphaOffset / 255);

		for (int i = 0; i < numSegmentPoint; i++)
		{
			float timeLife = m_timeLifeData[i] * 255;
			ColorTransform& currentColor = segmentPoints[i].m_color;
			if (i == 0)
			{
				m_offsetData.push_back((currentColor.m_redOffset - startColor.m_redOffset) / timeLife);
				m_offsetData.push_back((currentColor.m_greenOffset - startColor.m_greenOffset) / timeLife);
				m_offsetData.push_back((currentColor.m_blueOffset - startColor.m_blueOffset) / timeLife);
				m_offsetData.push_back((currentColor.m_alphaOffset - startColor.m_alphaOffset) / timeLife);
			}
			else
			{
				ColorTransform& previousColor = segmentPoints[i - 1].m_color;
				m_offsetData.push_back((currentColor.m_redOffset - previousColor.m_redOffset) / timeLife);
				m_offsetData.push_back((currentColor.m_greenOffset - previousColor.m_greenOffset) / timeLife);
				m_offsetData.push_back((currentColor.m_blueOffset - previousColor.m_blueOffset) / timeLife);
				m_offsetData.push_back((currentColor.m_alphaOffset - previousColor.m_alphaOffset) / timeLife);
			}
		}

		if (numSegmentPoint == 0)
		{
			m_offsetData.push_back((endColor.m_redOffset - startColor.m_redOffset) / 255);
			m_offsetData.push_back((endColor.m_greenOffset - startColor.m_greenOffset) / 255);
			m_offsetData.push_back((endColor.m_blueOffset - startColor.m_blueOffset) / 255);
			m_offsetData.push_back((endColor.m_alphaOffset - startColor.m_alphaOffset) / 255);
		}
		else
		{
			float timeLife = m_timeLifeData[numSegmentPoint] * 255;
			ColorTransform& previousColor = segmentPoints[numSegmentPoint - 1].m_color;
			m_offsetData.push_back((endColor.m_redOffset - previousColor.m_redOffset) / timeLife);
			m_offsetData.push_back((endColor.m_greenOffset - previousColor.m_greenOffset) / timeLife);
			m_offsetData.push_back((endColor.m_blueOffset - previousColor.m_blueOffset) / timeLife);
			m_offsetData.push_back((endColor.m_alphaOffset - previousColor.m_alphaOffset) / timeLife);
		}
	}

	// cut off the data
	m_timeLifeData.resize(4);
}