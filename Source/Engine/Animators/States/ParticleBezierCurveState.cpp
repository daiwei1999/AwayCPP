#include "ParticleBezierCurveState.h"
#include "ParticleBezierCurveNode.h"
#include "AnimationRegisterCache.h"
#include "AnimationSubGeometry.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

ParticleBezierCurveState::ParticleBezierCurveState(IAnimator* animator, ParticleBezierCurveNode* particleBezierCurveNode) : ParticleStateBase(animator, particleBezierCurveNode)
{

}

void ParticleBezierCurveState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	int controlIndex = regCache->getRegisterIndex(m_animationNode, ParticleBezierCurveNode::BEZIER_CONTROL_INDEX);
	int endIndex = regCache->getRegisterIndex(m_animationNode, ParticleBezierCurveNode::BEZIER_END_INDEX);

	ParticleBezierCurveNode* particleBezierCurveNode = static_cast<ParticleBezierCurveNode*>(m_animationNode);
	if (particleBezierCurveNode->getMode() == ParticlePropertiesMode::LOCAL_STATIC)
	{
		subGeometry->activateVertexBuffer(controlIndex, particleBezierCurveNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
		subGeometry->activateVertexBuffer(endIndex, particleBezierCurveNode->m_dataOffset + 3, context, VertexBufferFormat::FLOAT_3);
	}
	else
	{
		Vector3D* controlPoint = particleBezierCurveNode->m_controlPoint;
		Vector3D* endPoint = particleBezierCurveNode->m_endPoint;
		regCache->setVertexConst(controlIndex, controlPoint->m_x, controlPoint->m_y, controlPoint->m_z);
		regCache->setVertexConst(endIndex, endPoint->m_x, endPoint->m_y, endPoint->m_z);
	}
}