#include "ParticleUVState.h"
#include "ParticleUVNode.h"
#include "AnimationRegisterCache.h"
#include "MathConsts.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleUVState::ParticleUVState(IAnimator* animator, ParticleUVNode* particleUVNode) : ParticleStateBase(animator, particleUVNode)
{
	
}

void ParticleUVState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_needUVAnimation)
	{
		ParticleUVNode* particleUVNode = static_cast<ParticleUVNode*>(m_animationNode);
		int index = regCache->getRegisterIndex(m_animationNode, ParticleUVNode::UV_INDEX);

		if (particleUVNode->getMode() == ParticlePropertiesMode::GLOBAL)
		{
			switch (particleUVNode->m_formula)
			{
			case Formula::SINE_EASE:
				regCache->setVertexConst(index, MathConsts::TWO_PI / particleUVNode->m_cycle, particleUVNode->m_scale);
				break;
			case Formula::LINEAR_EASE:
				regCache->setVertexConst(index, 1 / particleUVNode->m_cycle, particleUVNode->m_scale);
				break;
			}
		}
		else
			subGeometry->activateVertexBuffer(index, particleUVNode->m_dataOffset, context, VertexBufferFormat::FLOAT_2);
	}
}