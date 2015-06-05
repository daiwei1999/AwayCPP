#include "ParticleOscillatorState.h"
#include "ParticleOscillatorNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

ParticleOscillatorState::ParticleOscillatorState(IAnimator* animator, ParticleOscillatorNode* particleOscillatorNode) : ParticleStateBase(animator, particleOscillatorNode)
{
	updateOscillatorData();
}

void ParticleOscillatorState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticleOscillatorNode* particleOscillatorNode = static_cast<ParticleOscillatorNode*>(m_animationNode);
	int index = regCache->getRegisterIndex(m_animationNode, ParticleOscillatorNode::OSCILLATOR_INDEX);

	if (particleOscillatorNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
		subGeometry->activateVertexBuffer(index, particleOscillatorNode->m_dataOffset, context, VertexBufferFormat::FLOAT_4);
	else
		regCache->setVertexConst(index, m_oscillatorData->m_x, m_oscillatorData->m_y, m_oscillatorData->m_z, m_oscillatorData->m_w);
}

void ParticleOscillatorState::updateOscillatorData()
{
	ParticleOscillatorNode* particleOscillatorNode = static_cast<ParticleOscillatorNode*>(m_animationNode);
	if (particleOscillatorNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		Vector3D* oscillator = particleOscillatorNode->m_oscillator;
		m_oscillatorData = new Vector3D(oscillator->m_x, oscillator->m_y, oscillator->m_z, MathConsts::TWO_PI / oscillator->m_w);
	}
}