#include "ParticleBezierCurveNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleBezierCurveState.h"

USING_AWAY_NAMESPACE

const int ParticleBezierCurveNode::BEZIER_CONTROL_INDEX = 0;
const int ParticleBezierCurveNode::BEZIER_END_INDEX = 1;
const std::string ParticleBezierCurveNode::BEZIER_CONTROL_VECTOR3D = "BezierControlVector3D";
const std::string ParticleBezierCurveNode::BEZIER_END_VECTOR3D = "BezierEndVector3D";

ParticleBezierCurveNode::ParticleBezierCurveNode(ParticlePropertiesMode mode, Vector3D* controlPoint, Vector3D* endPoint) : ParticleNodeBase("ParticleBezierCurve", mode, 6)
{
	m_controlPoint = controlPoint;
	m_endPoint = endPoint;
}

void ParticleBezierCurveNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int controlValue = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, BEZIER_CONTROL_INDEX, REGISTER_INDEX(controlValue));

	unsigned int endValue = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, BEZIER_END_INDEX, REGISTER_INDEX(endValue));

	unsigned int temp = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(temp, 1);
	unsigned int temp2 = regCache->getFreeVertexVectorTemp();
	regCache->removeVertexTempUsage(temp);

	code.sub(temp ^ Regs::x, regCache->m_vertexOneConst, regCache->m_vertexLife);
	code.mul(temp ^ Regs::y, regCache->m_vertexLife, regCache->m_vertexLife);
	code.mul(temp ^ Regs::z, regCache->m_vertexLife, temp ^ Regs::x);
	code.mul(temp ^ Regs::z, temp ^ Regs::z, regCache->m_vertexTwoConst);
	code.mul(temp2 ^ Regs::xyz, temp ^ Regs::z, controlValue);
	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget, temp2);
	code.mul(temp2 ^ Regs::xyz, temp ^ Regs::y, endValue);
	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget, temp2);

	if (regCache->m_needVelocity)
	{
		code.mul(temp ^ Regs::y, regCache->m_vertexLife, regCache->m_vertexTwoConst);
		code.sub(temp ^ Regs::z, regCache->m_vertexOneConst, temp ^ Regs::y);
		code.mul(temp ^ Regs::z, regCache->m_vertexTwoConst, temp ^ Regs::z);
		code.mul(temp2 ^ Regs::xyz, controlValue, temp ^ Regs::z);
		code.add(regCache->m_velocityTarget ^ Regs::xyz, regCache->m_velocityTarget, temp2);
		code.mul(temp2 ^ Regs::xyz, endValue, temp ^ Regs::y);
		code.add(regCache->m_velocityTarget ^ Regs::xyz, regCache->m_velocityTarget, temp2);
	}
}

AnimationStateBase* ParticleBezierCurveNode::createAnimationState(IAnimator* animator)
{
	return new ParticleBezierCurveState(animator, this);
}

void ParticleBezierCurveNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& bezierControl = param[BEZIER_CONTROL_VECTOR3D];
	Vector3D& bezierEnd = param[BEZIER_END_VECTOR3D];
	m_oneData[0] = bezierControl.m_x;
	m_oneData[1] = bezierControl.m_y;
	m_oneData[2] = bezierControl.m_z;
	m_oneData[3] = bezierEnd.m_x;
	m_oneData[4] = bezierEnd.m_y;
	m_oneData[5] = bezierEnd.m_z;
}