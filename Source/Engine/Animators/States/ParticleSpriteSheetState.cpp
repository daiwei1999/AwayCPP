#include "ParticleSpriteSheetState.h"
#include "ParticleSpriteSheetNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleSpriteSheetState::ParticleSpriteSheetState(IAnimator* animator, ParticleSpriteSheetNode* particleSpriteSheetNode) : ParticleStateBase(animator, particleSpriteSheetNode)
{
	updateSpriteSheetData();
}

void ParticleSpriteSheetState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_needUVAnimation)
	{
		ParticleSpriteSheetNode* particleSpriteSheetNode = static_cast<ParticleSpriteSheetNode*>(m_animationNode);
		regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleSpriteSheetNode::UV_INDEX_0), m_spriteSheetData[0], m_spriteSheetData[1], m_spriteSheetData[2], m_spriteSheetData[3]);

		if (particleSpriteSheetNode->m_usesCycle)
		{
			int index = regCache->getRegisterIndex(m_animationNode, ParticleSpriteSheetNode::UV_INDEX_1);
			if (particleSpriteSheetNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
				subGeometry->activateVertexBuffer(index, particleSpriteSheetNode->m_dataOffset, context, particleSpriteSheetNode->m_usesPhase ? VertexBufferFormat::FLOAT_3 : VertexBufferFormat::FLOAT_2);
			else
				regCache->setVertexConst(index, m_spriteSheetData[4], m_spriteSheetData[5]);
		}
	}
}

void ParticleSpriteSheetState::updateSpriteSheetData()
{
	ParticleSpriteSheetNode* particleSpriteSheetNode = static_cast<ParticleSpriteSheetNode*>(m_animationNode);
	m_spriteSheetData[0] = (float)particleSpriteSheetNode->m_numRows;
	m_spriteSheetData[1] = 1.f / particleSpriteSheetNode->m_numColumns;
	m_spriteSheetData[2] = 1.f / particleSpriteSheetNode->m_numRows;

	if (particleSpriteSheetNode->m_usesCycle)
	{
		m_spriteSheetData[4] = particleSpriteSheetNode->m_numRows / particleSpriteSheetNode->m_cycleDuration;
		m_spriteSheetData[5] = particleSpriteSheetNode->m_cycleDuration;
		if (particleSpriteSheetNode->m_usesPhase)
			m_spriteSheetData[6] = particleSpriteSheetNode->m_cyclePhase;
	}
}