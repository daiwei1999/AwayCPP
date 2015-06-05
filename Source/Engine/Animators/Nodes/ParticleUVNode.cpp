#include "ParticleUVNode.h"
#include "ParticleAnimationSet.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleUVState.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

const int ParticleUVNode::UV_INDEX = 0;
const std::string ParticleUVNode::UV_VECTOR3D = "UVVector3D";

ParticleUVNode::ParticleUVNode(ParticlePropertiesMode mode, float cycle, float scale, Axis axis, Formula formula) : ParticleNodeBase("ParticleUV", mode, 4, ParticleAnimationSet::POST_PRIORITY + 1)
{
	m_cycle = cycle;
	m_scale = scale;
	m_axis = axis;
	m_formula = formula;
}

void ParticleUVNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	if (!regCache->m_needUVAnimation)
		return;

	unsigned int UVReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, UV_INDEX, REGISTER_INDEX(UVReg));

	unsigned int axisIndex = (m_axis == Axis::U_AXIS ? Regs::x : (m_axis == Axis::V_AXIS ? Regs::y : Regs::z));
	unsigned int target = regCache->m_uvTarget ^ axisIndex;
	unsigned int temp = regCache->getFreeVertexSingleTemp();

	if (m_scale != 1)
		code.mul(target, target, UVReg ^ Regs::y);

	switch (m_formula)
	{
	case Formula::SINE_EASE:
		code.mul(temp, regCache->m_vertexTime, UVReg ^ Regs::x);
		code.sin(temp, temp);
		break;
	case Formula::LINEAR_EASE:
		code.mul(temp, regCache->m_vertexTime, UVReg ^ Regs::x);
		break;
	}

	code.add(target, target, temp);
}

AnimationStateBase* ParticleUVNode::createAnimationState(IAnimator* animator)
{
	return new ParticleUVState(animator, this);
}

void ParticleUVNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	particleAnimationSet->m_hasUVNode = true;
}

void ParticleUVNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& uvData = param[UV_VECTOR3D];
	switch (m_formula)
	{
	case Formula::SINE_EASE:
		m_oneData[0] = MathConsts::TWO_PI / uvData.m_x;
		break;
	case Formula::LINEAR_EASE:
		m_oneData[0] = 1 / uvData.m_x;
		break;
	}

	m_oneData[1] = uvData.m_y;
}