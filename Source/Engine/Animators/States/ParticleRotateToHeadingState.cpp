#include "ParticleRotateToHeadingState.h"
#include "ParticleRotateToHeadingNode.h"
#include "AnimationRegisterCache.h"
#include "Matrix3D.h"
#include "IRenderable.h"
#include "Camera3D.h"

USING_AWAY_NAMESPACE

ParticleRotateToHeadingState::ParticleRotateToHeadingState(IAnimator* animator, ParticleRotateToHeadingNode* particleRotateToHeadingNode) : ParticleStateBase(animator, particleRotateToHeadingNode)
{

}

void ParticleRotateToHeadingState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	if (regCache->m_hasBillboard)
	{
		Matrix3D matrix(renderable->getRenderSceneTransform(camera));
		matrix.append(camera->getInverseSceneTransform());
		regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleRotateToHeadingNode::MATRIX_INDEX), matrix, true);
	}
}