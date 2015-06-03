#include "ParticleRotateToPositionState.h"
#include "ParticleRotateToPositionNode.h"
#include "AnimationRegisterCache.h"
#include "IRenderable.h"
#include "Camera3D.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleRotateToPositionState::ParticleRotateToPositionState(IAnimator* animator, ParticleRotateToPositionNode* particleRotateToPositionNode) : ParticleStateBase(animator, particleRotateToPositionNode)
{
	
}

void ParticleRotateToPositionState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_hasBillboard)
	{
		Matrix3D matrix(renderable->getRenderSceneTransform(camera));
		matrix.append(camera->getInverseSceneTransform());
		regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleRotateToPositionNode::MATRIX_INDEX), matrix);
	}

	int index = regCache->getRegisterIndex(m_animationNode, ParticleRotateToPositionNode::POSITION_INDEX);
	ParticleRotateToPositionNode* particleRotateToPositionNode = static_cast<ParticleRotateToPositionNode*>(m_animationNode);
	if (particleRotateToPositionNode->getMode() == ParticlePropertiesMode::GLOBAL)
	{
		Vector3D offset;
		Matrix3D::transformVector(renderable->getInverseSceneTransform(), particleRotateToPositionNode->m_position, &offset);
		regCache->setVertexConst(index, offset.m_x, offset.m_y, offset.m_z);
	}
	else
		subGeometry->activateVertexBuffer(index, particleRotateToPositionNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
}