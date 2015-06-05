#include "ParticleInitialColorNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleInitialColorState.h"
#include "ParticleAnimationSet.h"

USING_AWAY_NAMESPACE

const int ParticleInitialColorNode::MULTIPLIER_INDEX = 0;
const int ParticleInitialColorNode::OFFSET_INDEX = 1;
const std::string ParticleInitialColorNode::COLOR_INITIAL_MULTIPLIER = "ColorInitialMultiplier";
const std::string ParticleInitialColorNode::COLOR_INITIAL_OFFSET = "ColorInitialOffset";

ParticleInitialColorNode::ParticleInitialColorNode(ParticlePropertiesMode mode, bool usesMultiplier, bool usesOffset, ColorTransform* initialColor) : ParticleNodeBase("ParticleInitialColor", mode, (usesMultiplier && usesOffset) ? 8 : 4, ParticleAnimationSet::COLOR_PRIORITY)
{
	m_usesMultiplier = usesMultiplier;
	m_usesOffset = usesOffset;
	m_initialColor = initialColor;
}

void ParticleInitialColorNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	if (!regCache->m_needFragmentAnimation)
		return;

	if (m_usesMultiplier)
	{
		unsigned int multiplierReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
		regCache->setRegisterIndex(this, MULTIPLIER_INDEX, REGISTER_INDEX(multiplierReg));

		code.mul(regCache->m_colorMulTarget, regCache->m_colorMulTarget, multiplierReg);
	}

	if (m_usesOffset)
	{
		unsigned int offsetReg = (m_mode == ParticlePropertiesMode::LOCAL_STATIC) ? regCache->getFreeVertexAttribute() : regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, OFFSET_INDEX, REGISTER_INDEX(offsetReg));

		code.add(regCache->m_colorAddTarget, regCache->m_colorAddTarget, offsetReg);
	}
}

AnimationStateBase* ParticleInitialColorNode::createAnimationState(IAnimator* animator)
{
	return new ParticleInitialColorState(animator, this);
}

void ParticleInitialColorNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	if (m_usesMultiplier)
		particleAnimationSet->m_hasColorMulNode = true;

	if (m_usesOffset)
		particleAnimationSet->m_hasColorAddNode = true;
}

void ParticleInitialColorNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	int offset = 0;
	Vector3D& initialColorMultiplier = param[COLOR_INITIAL_MULTIPLIER];
	Vector3D& initialColorOffset = param[COLOR_INITIAL_OFFSET];

	if (m_usesMultiplier)
	{
		m_oneData[0] = initialColorMultiplier.m_x;
		m_oneData[1] = initialColorMultiplier.m_y;
		m_oneData[2] = initialColorMultiplier.m_z;
		m_oneData[3] = initialColorMultiplier.m_w;
		offset = 4;
	}
	if (m_usesOffset)
	{
		m_oneData[offset] = initialColorOffset.m_x / 255;
		m_oneData[offset + 1] = initialColorOffset.m_y / 255;
		m_oneData[offset + 2] = initialColorOffset.m_z / 255;
		m_oneData[offset + 3] = initialColorOffset.m_w / 255;
	}
}