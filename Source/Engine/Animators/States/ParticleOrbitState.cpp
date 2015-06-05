#include "ParticleOrbitState.h"
#include "ParticleOrbitNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"
#include "Matrix3D.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

ParticleOrbitState::ParticleOrbitState(IAnimator* animator, ParticleOrbitNode* particleOrbitNode) : ParticleStateBase(animator, particleOrbitNode)
{
	updateOrbitData();
}

void ParticleOrbitState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticleOrbitNode* particleOrbitNode = static_cast<ParticleOrbitNode*>(m_animationNode);
	int index = regCache->getRegisterIndex(m_animationNode, ParticleOrbitNode::ORBIT_INDEX);

	if (particleOrbitNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
	{
		if (particleOrbitNode->m_usesPhase)
			subGeometry->activateVertexBuffer(index, particleOrbitNode->m_dataOffset, context, VertexBufferFormat::FLOAT_4);
		else
			subGeometry->activateVertexBuffer(index, particleOrbitNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
	}
	else
		regCache->setVertexConst(index, m_orbitData->m_x, m_orbitData->m_y, m_orbitData->m_z, m_orbitData->m_w);

	if (particleOrbitNode->m_usesEulers)
		regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleOrbitNode::EULERS_INDEX), *m_eulersMatrix);
}

void ParticleOrbitState::updateOrbitData()
{
	ParticleOrbitNode* particleOrbitNode = static_cast<ParticleOrbitNode*>(m_animationNode);
	if (particleOrbitNode->m_usesEulers)
	{
		m_eulersMatrix = new Matrix3D();
		Vector3D* eulers = particleOrbitNode->m_eulers;
		m_eulersMatrix->appendRotation(eulers->m_x, Vector3D::X_AXIS);
		m_eulersMatrix->appendRotation(eulers->m_y, Vector3D::Y_AXIS);
		m_eulersMatrix->appendRotation(eulers->m_z, Vector3D::Z_AXIS);
	}

	if (particleOrbitNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		m_orbitData = new Vector3D(particleOrbitNode->m_radius, 0, particleOrbitNode->m_radius * MathConsts::TWO_PI, particleOrbitNode->m_cyclePhase * MathConsts::DEGREES_TO_RADIANS);
		if (particleOrbitNode->m_usesCycle)
			m_orbitData->m_y = MathConsts::TWO_PI / particleOrbitNode->m_cycleDuration;
		else
			m_orbitData->m_y = MathConsts::TWO_PI;
	}
}