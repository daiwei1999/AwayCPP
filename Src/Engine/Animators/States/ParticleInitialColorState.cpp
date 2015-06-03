#include "ParticleInitialColorState.h"
#include "ParticleInitialColorNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "ColorTransform.h"

USING_AWAY_NAMESPACE

ParticleInitialColorState::ParticleInitialColorState(IAnimator* animator, ParticleInitialColorNode* particleInitialColorNode) : ParticleStateBase(animator, particleInitialColorNode)
{
	updateColorData();
}

void ParticleInitialColorState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_needFragmentAnimation)
	{
		ParticleInitialColorNode* particleInitialColorNode = static_cast<ParticleInitialColorNode*>(m_animationNode);
		if (particleInitialColorNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
		{
			int dataOffset = particleInitialColorNode->m_dataOffset;
			if (particleInitialColorNode->m_usesMultiplier)
			{
				subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleInitialColorNode::MULTIPLIER_INDEX), dataOffset, context, VertexBufferFormat::FLOAT_4);
				dataOffset += 4;
			}

			if (particleInitialColorNode->m_usesOffset)
				subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleInitialColorNode::OFFSET_INDEX), dataOffset, context, VertexBufferFormat::FLOAT_4);
		}
		else
		{
			if (particleInitialColorNode->m_usesMultiplier)
				regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleInitialColorNode::MULTIPLIER_INDEX), m_multiplierData->m_x, m_multiplierData->m_y, m_multiplierData->m_z, m_multiplierData->m_w);

			if (particleInitialColorNode->m_usesOffset)
				regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleInitialColorNode::OFFSET_INDEX), m_offsetData->m_x, m_offsetData->m_y, m_offsetData->m_z, m_offsetData->m_w);
		}
	}
}

void ParticleInitialColorState::updateColorData()
{
	ParticleInitialColorNode* particleInitialColorNode = static_cast<ParticleInitialColorNode*>(m_animationNode);
	if (particleInitialColorNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		ColorTransform* initialColor = particleInitialColorNode->m_initialColor;
		if (particleInitialColorNode->m_usesMultiplier)
			m_multiplierData = new Vector3D(initialColor->m_redMultiplier, initialColor->m_greenMultiplier, initialColor->m_blueMultiplier, initialColor->m_alphaMultiplier);

		if (particleInitialColorNode->m_usesOffset)
			m_offsetData = new Vector3D(initialColor->m_redOffset / 255, initialColor->m_greenOffset / 255, initialColor->m_blueOffset / 255, initialColor->m_alphaOffset / 255);
	}
}