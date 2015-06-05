#include "ParticleScaleState.h"
#include "ParticleScaleNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

ParticleScaleState::ParticleScaleState(IAnimator* animator, ParticleScaleNode* particleScaleNode) : ParticleStateBase(animator, particleScaleNode)
{
	updateScaleData();
}

void ParticleScaleState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	int index = regCache->getRegisterIndex(m_animationNode, ParticleScaleNode::SCALE_INDEX);

	ParticleScaleNode* particleScaleNode = static_cast<ParticleScaleNode*>(m_animationNode);
	if (particleScaleNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
	{
		if (particleScaleNode->m_usesCycle)
		{
			if (particleScaleNode->m_usesPhase)
				subGeometry->activateVertexBuffer(index, particleScaleNode->m_dataOffset, context, VertexBufferFormat::FLOAT_4);
			else
				subGeometry->activateVertexBuffer(index, particleScaleNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
		}
		else
			subGeometry->activateVertexBuffer(index, particleScaleNode->m_dataOffset, context, VertexBufferFormat::FLOAT_2);
	}
	else
		regCache->setVertexConst(index, m_scaleData->m_x, m_scaleData->m_y, m_scaleData->m_z, m_scaleData->m_w);
}

void ParticleScaleState::updateScaleData()
{
	ParticleScaleNode* particleScaleNode = static_cast<ParticleScaleNode*>(m_animationNode);
	if (particleScaleNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		float minScale = particleScaleNode->m_minScale;
		float maxScale = particleScaleNode->m_maxScale;
		if (particleScaleNode->m_usesCycle)
			m_scaleData = new Vector3D((minScale + maxScale) / 2, std::abs(minScale - maxScale) / 2, MathConsts::TWO_PI / particleScaleNode->m_cycleDuration, particleScaleNode->m_cyclePhase * MathConsts::DEGREES_TO_RADIANS);
		else
			m_scaleData = new Vector3D(minScale, maxScale - minScale, 0, 0);
	}
}