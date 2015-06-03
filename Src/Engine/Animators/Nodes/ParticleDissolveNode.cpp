#include "ParticleDissolveNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleDissolveState.h"

USING_AWAY_NAMESPACE

const int ParticleDissolveNode::DISSOLVE_INDEX = 0;

ParticleDissolveNode::ParticleDissolveNode(float speed, float timeOffset) : ParticleNodeBase("ParticleDissolve", ParticlePropertiesMode::GLOBAL, 0)
{
	m_speed = speed;
	m_timeOffset = timeOffset;
}

void ParticleDissolveNode::getAGALFragmentCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int dataReg = regCache->getFreeFragmentConstant();
	regCache->setRegisterIndex(this, DISSOLVE_INDEX, REGISTER_INDEX(dataReg));

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	code.dp3(temp ^ Regs::x, dataReg, regCache->m_colorTarget); // calculate luminance
	code.sub(temp ^ Regs::x, dataReg ^ Regs::w, temp ^ Regs::x); // 1 - dt - luminance
	code.kil(temp ^ Regs::x);
}

AnimationStateBase* ParticleDissolveNode::createAnimationState(IAnimator* animator)
{
	return new ParticleDissolveState(animator, this);
}