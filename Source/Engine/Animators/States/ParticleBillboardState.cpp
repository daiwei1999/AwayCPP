#include "ParticleBillboardState.h"
#include "ParticleBillboardNode.h"
#include "IRenderable.h"
#include "Camera3D.h"
#include "AnimationRegisterCache.h"

USING_AWAY_NAMESPACE

ParticleBillboardState::ParticleBillboardState(IAnimator* animator, ParticleBillboardNode* particleBillboardNode) : ParticleStateBase(animator, particleBillboardNode)
{

}

void ParticleBillboardState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	Matrix3D matrix;
	float factor = 1;
	Vector3D pos, look, right;
	Matrix3D& transform = renderable->getRenderSceneTransform(camera);
	Vector3D* billboardAxis = static_cast<ParticleBillboardNode*>(m_animationNode)->m_billboardAxis;

	if (billboardAxis)
	{
		transform.copyColumnTo(3, pos);
		camera->getSceneTransform().copyColumnTo(3, look);
		look.decrementBy(pos);

		look.crossProduct(*billboardAxis, right);
		right.normalize();
		billboardAxis->crossProduct(right, look);
		look.normalize();

		matrix.copyColumnFrom(0, right);
		matrix.copyColumnFrom(1, *billboardAxis);
		matrix.copyColumnFrom(2, look);
		matrix.copyColumnFrom(3, pos);

		transform.copyColumnTo(0, pos);
		transform.copyColumnTo(1, look);
		transform.copyColumnTo(2, right);

		if (pos.m_x * (look.m_y * right.m_z - look.m_z * right.m_y) - pos.m_y * (look.m_x * right.m_z - look.m_z * right.m_x) + pos.m_z * (look.m_x * right.m_y - look.m_y * right.m_x) < 0)
			factor = -1;

		pos.scaleBy(1 / pos.getLength());
		look.scaleBy(1 / look.getLength());
		right.scaleBy(factor / right.getLength());

		Matrix3D temp;
		temp.copyRowFrom(0, pos);
		temp.copyRowFrom(1, look);
		temp.copyRowFrom(2, right);
		matrix.append(temp, true);
	}
	else
	{
		matrix.copyFrom(transform);
		matrix.append(camera->getInverseSceneTransform());

		matrix.copyColumnTo(0, pos);
		matrix.copyColumnTo(1, look);
		matrix.copyColumnTo(2, right);

		if (pos.m_x * (look.m_y * right.m_z - look.m_z * right.m_y) - pos.m_y * (look.m_x * right.m_z - look.m_z * right.m_x) + pos.m_z * (look.m_x * right.m_y - look.m_y * right.m_x) < 0)
			factor = -1;

		pos.scaleBy(1 / pos.getLength());
		look.scaleBy(1 / look.getLength());
		right.scaleBy(factor / right.getLength());

		matrix.copyRowFrom(0, pos);
		matrix.copyRowFrom(1, look);
		matrix.copyRowFrom(2, right);
	}

	regCache->setVertexConst(regCache->getRegisterIndex(m_animationNode, ParticleBillboardNode::MATRIX_INDEX), matrix, true);
}