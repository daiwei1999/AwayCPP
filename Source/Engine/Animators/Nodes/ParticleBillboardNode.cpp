#include "ParticleBillboardNode.h"
#include "ParticleBillboardState.h"
#include "ParticleAnimationSet.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

const int ParticleBillboardNode::MATRIX_INDEX = 0;

ParticleBillboardNode::ParticleBillboardNode(Vector3D* billboardAxis) : ParticleNodeBase("ParticleBillboard", ParticlePropertiesMode::GLOBAL, 0, 4)
{
	m_billboardAxis = billboardAxis;
}

void ParticleBillboardNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int rotationMatrixReg = regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->setRegisterIndex(this, MATRIX_INDEX, REGISTER_INDEX(rotationMatrixReg));

	code.m33(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, rotationMatrixReg);

	for (auto rotationReg : regCache->m_rotationRegisters)
		code.m33(rotationReg ^ Regs::xyz, rotationReg, rotationMatrixReg);
}

AnimationStateBase* ParticleBillboardNode::createAnimationState(IAnimator* animator)
{
	return new ParticleBillboardState(animator, this);
}

void ParticleBillboardNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	particleAnimationSet->m_hasBillboard = true;
}