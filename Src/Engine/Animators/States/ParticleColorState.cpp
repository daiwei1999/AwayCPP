#include "ParticleColorState.h"
#include "ParticleColorNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "MathConsts.h"
#include "ColorTransform.h"

USING_AWAY_NAMESPACE

ParticleColorState::ParticleColorState(IAnimator* animator, ParticleColorNode* particleColorNode) : ParticleStateBase(animator, particleColorNode)
{
	updateColorData();
}

void ParticleColorState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_needFragmentAnimation)
	{
		ParticleColorNode* particleColorNode = static_cast<ParticleColorNode*>(m_animationNode);
		if (particleColorNode->m_usesCycle)
			regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::CYCLE_INDEX), m_cycleData->m_x, m_cycleData->m_y, m_cycleData->m_z, m_cycleData->m_w);

		int dataOffset = particleColorNode->m_dataOffset;
		if (particleColorNode->m_usesMultiplier)
		{
			if (particleColorNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
			{
				subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::START_MULTIPLIER_INDEX), dataOffset, context, VertexBufferFormat::FLOAT_4);
				dataOffset += 4;
				subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::DELTA_MULTIPLIER_INDEX), dataOffset, context, VertexBufferFormat::FLOAT_4);
				dataOffset += 4;
			}
			else
			{
				regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::START_MULTIPLIER_INDEX), m_startMultiplierData->m_x, m_startMultiplierData->m_y, m_startMultiplierData->m_z, m_startMultiplierData->m_w);
				regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::DELTA_MULTIPLIER_INDEX), m_deltaMultiplierData->m_x, m_deltaMultiplierData->m_y, m_deltaMultiplierData->m_z, m_deltaMultiplierData->m_w);
			}
		}

		if (particleColorNode->m_usesOffset)
		{
			if (particleColorNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
			{
				subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::START_OFFSET_INDEX), dataOffset, context, VertexBufferFormat::FLOAT_4);
				dataOffset += 4;
				subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::DELTA_OFFSET_INDEX), dataOffset, context, VertexBufferFormat::FLOAT_4);
				dataOffset += 4;
			}
			else
			{
				regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::START_OFFSET_INDEX), m_startOffsetData->m_x, m_startOffsetData->m_y, m_startOffsetData->m_z, m_startOffsetData->m_w);
				regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleColorNode::DELTA_OFFSET_INDEX), m_deltaOffsetData->m_x, m_deltaOffsetData->m_y, m_deltaOffsetData->m_z, m_deltaOffsetData->m_w);
			}
		}
	}
}

void ParticleColorState::updateColorData()
{
	ParticleColorNode* particleColorNode = static_cast<ParticleColorNode*>(m_animationNode);
	if (particleColorNode->m_usesCycle)
		m_cycleData = new Vector3D(MathConsts::TWO_PI / particleColorNode->m_cycleDuration, particleColorNode->m_cyclePhase * MathConsts::DEGREES_TO_RADIANS, 0, 0);

	if (particleColorNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		ColorTransform* startColor = particleColorNode->m_startColor;
		ColorTransform* endColor = particleColorNode->m_endColor;
		if (particleColorNode->m_usesCycle)
		{
			if (particleColorNode->m_usesMultiplier)
			{
				m_startMultiplierData = new Vector3D((startColor->m_redMultiplier + endColor->m_redMultiplier) / 2, (startColor->m_greenMultiplier + endColor->m_greenMultiplier) / 2, (startColor->m_blueMultiplier + endColor->m_blueMultiplier) / 2, (startColor->m_alphaMultiplier + endColor->m_alphaMultiplier) / 2);
				m_deltaMultiplierData = new Vector3D((endColor->m_redMultiplier - startColor->m_redMultiplier) / 2, (endColor->m_greenMultiplier - startColor->m_greenMultiplier) / 2, (endColor->m_blueMultiplier - startColor->m_blueMultiplier) / 2, (endColor->m_alphaMultiplier - startColor->m_alphaMultiplier) / 2);
			}

			if (particleColorNode->m_usesOffset)
			{
				m_startOffsetData = new Vector3D((startColor->m_redOffset + endColor->m_redOffset) / 510, (startColor->m_greenOffset + endColor->m_greenOffset) / 510, (startColor->m_blueOffset + endColor->m_blueOffset) / 510, (startColor->m_alphaOffset + endColor->m_alphaOffset) / 510);
				m_deltaOffsetData = new Vector3D((endColor->m_redOffset - startColor->m_redOffset) / 510, (endColor->m_greenOffset - startColor->m_greenOffset) / 510, (endColor->m_blueOffset - startColor->m_blueOffset) / 510, (endColor->m_alphaOffset - startColor->m_alphaOffset) / 510);
			}
		}
		else
		{
			if (particleColorNode->m_usesMultiplier)
			{
				m_startMultiplierData = new Vector3D(startColor->m_redMultiplier, startColor->m_greenMultiplier, startColor->m_blueMultiplier, startColor->m_alphaMultiplier);
				m_deltaMultiplierData = new Vector3D(endColor->m_redMultiplier - startColor->m_redMultiplier, endColor->m_greenMultiplier - startColor->m_greenMultiplier, endColor->m_blueMultiplier - startColor->m_blueMultiplier, endColor->m_alphaMultiplier - startColor->m_alphaMultiplier);
			}

			if (particleColorNode->m_usesOffset)
			{
				m_startOffsetData = new Vector3D(startColor->m_redOffset / 255, startColor->m_greenOffset / 255, startColor->m_blueOffset / 255, startColor->m_alphaOffset / 255);
				m_deltaOffsetData = new Vector3D((endColor->m_redOffset - startColor->m_redOffset) / 255, (endColor->m_greenOffset - startColor->m_greenOffset) / 255, (endColor->m_blueOffset - startColor->m_blueOffset) / 255, (endColor->m_alphaOffset - startColor->m_alphaOffset) / 255);
			}
		}
	}
}