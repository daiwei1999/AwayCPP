#include "ParticleSegmentedScaleState.h"
#include "ParticleSegmentedScaleNode.h"
#include "AnimationRegisterCache.h"

USING_AWAY_NAMESPACE

ParticleSegmentedScaleState::ParticleSegmentedScaleState(IAnimator* animator, ParticleSegmentedScaleNode* particleSegmentedScaleNode) : ParticleStateBase(animator, particleSegmentedScaleNode)
{
	updateScaleData();
}

void ParticleSegmentedScaleState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleSegmentedScaleNode::START_INDEX), m_scaleData);
}

void ParticleSegmentedScaleState::updateScaleData()
{
	ParticleSegmentedScaleNode* particleSegmentedScaleNode = static_cast<ParticleSegmentedScaleNode*>(m_animationNode);
	Vector3D& startScale = particleSegmentedScaleNode->m_startScale;
	Vector3D& endScale = particleSegmentedScaleNode->m_endScale;
	std::vector<Vector3D>& segmentPoints = particleSegmentedScaleNode->m_segmentScales;

	std::vector<float> timeLifeData;
	int numSegmentPoint = segmentPoints.size();
	for (int i = 0; i < numSegmentPoint; i++)
	{
		if (i == 0)
			timeLifeData.push_back(segmentPoints[i].m_w);
		else
			timeLifeData.push_back(segmentPoints[i].m_w - segmentPoints[i - 1].m_w);
	}

	if (numSegmentPoint == 0)
		timeLifeData.push_back(1);
	else
		timeLifeData.push_back(1 - segmentPoints[numSegmentPoint - 1].m_w);

	m_scaleData.clear();
	m_scaleData.push_back(startScale.m_x);
	m_scaleData.push_back(startScale.m_y);
	m_scaleData.push_back(startScale.m_z);
	m_scaleData.push_back(0);

	for (int i = 0; i < numSegmentPoint; i++)
	{
		float timeLife = timeLifeData[i];
		Vector3D& currentScale = segmentPoints[i];
		if (i == 0)
		{
			m_scaleData.push_back((currentScale.m_x - startScale.m_x) / timeLife);
			m_scaleData.push_back((currentScale.m_y - startScale.m_y) / timeLife);
			m_scaleData.push_back((currentScale.m_z - startScale.m_z) / timeLife);
			m_scaleData.push_back(timeLife);
		}
		else
		{
			Vector3D& previousScale = segmentPoints[i - 1];
			m_scaleData.push_back((currentScale.m_x - previousScale.m_x) / timeLife);
			m_scaleData.push_back((currentScale.m_y - previousScale.m_y) / timeLife);
			m_scaleData.push_back((currentScale.m_z - previousScale.m_z) / timeLife);
			m_scaleData.push_back(timeLife);
		}
	}

	if (numSegmentPoint == 0)
	{
		m_scaleData.push_back(endScale.m_x - startScale.m_x);
		m_scaleData.push_back(endScale.m_y - startScale.m_y);
		m_scaleData.push_back(endScale.m_z - startScale.m_z);
		m_scaleData.push_back(1);
	}
	else
	{
		float timeLife = timeLifeData[numSegmentPoint];
		Vector3D& previousScale = segmentPoints[numSegmentPoint - 1];
		m_scaleData.push_back((endScale.m_x - previousScale.m_x) / timeLife);
		m_scaleData.push_back((endScale.m_y - previousScale.m_y) / timeLife);
		m_scaleData.push_back((endScale.m_z - previousScale.m_z) / timeLife);
		m_scaleData.push_back(timeLife);
	}
}