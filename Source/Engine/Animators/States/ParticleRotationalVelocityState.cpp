#include "ParticleRotationalVelocityState.h"
#include "ParticleRotationalVelocityNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

ParticleRotationalVelocityState::ParticleRotationalVelocityState(IAnimator* animator, ParticleRotationalVelocityNode* particleRotationalVelocityNode) : ParticleStateBase(animator, particleRotationalVelocityNode)
{
	updateRotationalVelocityData();
}

void ParticleRotationalVelocityState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticleRotationalVelocityNode* particleRotationalVelocityNode = static_cast<ParticleRotationalVelocityNode*>(m_animationNode);
	int index = regCache->getRegisterIndex(m_animationNode, ParticleRotationalVelocityNode::ROTATIONALVELOCITY_INDEX);
	if (particleRotationalVelocityNode->getMode() == ParticlePropertiesMode::GLOBAL)
		regCache->setVertexConst(index, m_rotationalVelocityData->m_x, m_rotationalVelocityData->m_y, m_rotationalVelocityData->m_z, m_rotationalVelocityData->m_w);
	else
		subGeometry->activateVertexBuffer(index, particleRotationalVelocityNode->m_dataOffset, context, VertexBufferFormat::FLOAT_4);
}

void ParticleRotationalVelocityState::updateRotationalVelocityData()
{
	ParticleRotationalVelocityNode* particleRotationalVelocityNode = static_cast<ParticleRotationalVelocityNode*>(m_animationNode);
	if (particleRotationalVelocityNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		Vector3D rotation(*particleRotationalVelocityNode->m_rotationalVelocity);
		if (rotation.getLength() <= 0)
			rotation.m_z = 1;
		else
			rotation.normalize();

		m_rotationalVelocityData = new Vector3D(rotation.m_x, rotation.m_y, rotation.m_z, MathConsts::PI / rotation.m_w);
	}
}