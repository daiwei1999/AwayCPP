#include "ParticleRotateToHeadingNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleRotateToHeadingState.h"
#include "ParticleAnimationSet.h"

USING_AWAY_NAMESPACE

const int ParticleRotateToHeadingNode::MATRIX_INDEX = 0;

ParticleRotateToHeadingNode::ParticleRotateToHeadingNode() : ParticleNodeBase("ParticleRotateToHeading", ParticlePropertiesMode::GLOBAL, 0, 3)
{

}

void ParticleRotateToHeadingNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	if (regCache->m_hasBillboard)
	{
		unsigned int temp1 = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp1, 1);
		unsigned int temp2 = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(temp2, 1);
		unsigned int temp3 = regCache->getFreeVertexVectorTemp();
		regCache->removeVertexTempUsage(temp1);
		regCache->removeVertexTempUsage(temp2);

		unsigned int rotationMatrixReg = regCache->getFreeVertexConstant();
		regCache->getFreeVertexConstant();
		regCache->getFreeVertexConstant();
		regCache->setRegisterIndex(this, MATRIX_INDEX, REGISTER_INDEX(rotationMatrixReg));

		// process the velocity
		code.m33(temp1 ^ Regs::xyz, regCache->m_velocityTarget, rotationMatrixReg);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.mov(temp3 ^ Regs::xy, temp1);
		code.nrm(temp3 ^ Regs::xyz, temp3);

		// only process z axis
		code.mov(temp2, regCache->m_vertexZeroConst);
		code.mov(temp2 ^ Regs::x, temp3 ^ Regs::y);
		code.mov(temp2 ^ Regs::y, temp3 ^ Regs::x);
		code.mov(temp1, regCache->m_vertexZeroConst);
		code.mov(temp1 ^ Regs::x, temp3 ^ Regs::x);
		code.neg(temp1 ^ Regs::y, temp3 ^ Regs::y);
		code.mov(temp3, regCache->m_vertexZeroConst);
		code.mov(temp3 ^ Regs::z, regCache->m_vertexOneConst);
		code.m33(regCache->m_scaleAndRotateTarget ^ Regs::xyz, regCache->m_scaleAndRotateTarget, temp1);
		for (auto rotationReg : regCache->m_rotationRegisters)
			code.m33(rotationReg ^ Regs::xyz, rotationReg, temp1);
	}
	else
	{
		unsigned int nrmVel = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(nrmVel, 1);
		unsigned int xAxis = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(xAxis, 1);
		unsigned int R = regCache->getFreeVertexVectorTemp();
		regCache->addVertexTempUsages(R, 1);
		unsigned int R_rev = regCache->getFreeVertexVectorTemp();
		regCache->removeVertexTempUsage(nrmVel);
		regCache->removeVertexTempUsage(xAxis);
		regCache->removeVertexTempUsage(R);

		unsigned int cos = R ^ Regs::w;
		unsigned int sin = R_rev ^ Regs::w;
		unsigned int cos2 = nrmVel ^ Regs::w;
		code.mov(xAxis ^ Regs::x, regCache->m_vertexOneConst);
		code.mov(xAxis ^ Regs::yz, regCache->m_vertexZeroConst);
		code.nrm(nrmVel ^ Regs::xyz, regCache->m_velocityTarget);
		code.dp3(cos2, nrmVel, xAxis);
		code.crs(nrmVel ^ Regs::xyz, xAxis, nrmVel);
		code.nrm(nrmVel ^ Regs::xyz, nrmVel);

		// use R as temp to judge if nrm is (0,0,0), if nrm is (0,0,0) than change it to (0,0,1).
		code.dp3(R ^ Regs::x, nrmVel, nrmVel);
		code.sge(R ^ Regs::x, regCache->m_vertexZeroConst, R ^ Regs::x);
		code.add(nrmVel ^ Regs::z, nrmVel ^ Regs::z, R ^ Regs::x);

		code.add(sin, cos2, regCache->m_vertexOneConst);
		code.div(sin, sin, regCache->m_vertexTwoConst);
		code.sqt(cos, sin);

		code.sub(sin, regCache->m_vertexOneConst, cos2);
		code.div(sin, sin, regCache->m_vertexTwoConst);
		code.sqt(sin, sin);

		code.mul(R ^ Regs::xyz, sin, nrmVel);

		code.mul(R_rev ^ Regs::xyz, sin, nrmVel);
		code.neg(R_rev ^ Regs::xyz, R_rev);

		// nrmVel and xAxis are used as temp register
		code.crs(nrmVel ^ Regs::xyz, R, regCache->m_scaleAndRotateTarget);

		// use cos as R.w
		code.mul(xAxis ^ Regs::xyz, cos, regCache->m_scaleAndRotateTarget);
		code.add(nrmVel ^ Regs::xyz, nrmVel, xAxis);
		code.dp3(xAxis ^ Regs::w, R, regCache->m_scaleAndRotateTarget);
		code.neg(cos2, xAxis ^ Regs::w);

		code.crs(R ^ Regs::xyz, nrmVel, R_rev);
		code.mul(xAxis, nrmVel, cos);
		code.add(R ^ Regs::xyz, R, xAxis);
		code.mul(xAxis ^ Regs::xyz, cos2, R_rev);
		code.add(regCache->m_scaleAndRotateTarget ^ Regs::xyz, R, xAxis);

		for (auto rotationReg : regCache->m_rotationRegisters)
		{
			// just repeat the calculate above, because of the limited registers, no need to optimize
			code.mov(xAxis ^ Regs::x, regCache->m_vertexOneConst);
			code.mov(xAxis ^ Regs::yz, regCache->m_vertexZeroConst);
			code.nrm(nrmVel ^ Regs::xyz, regCache->m_velocityTarget);
			code.dp3(cos2, nrmVel, xAxis);
			code.crs(nrmVel ^ Regs::xyz, xAxis, nrmVel);
			code.nrm(nrmVel ^ Regs::xyz, nrmVel);
			code.dp3(R ^ Regs::x, nrmVel, nrmVel);
			code.sge(R ^ Regs::x, regCache->m_vertexZeroConst, R ^ Regs::x);
			code.add(nrmVel ^ Regs::z, R ^ Regs::x, nrmVel ^ Regs::z);
			code.add(sin, cos2, regCache->m_vertexOneConst);
			code.div(sin, sin, regCache->m_vertexTwoConst);
			code.sqt(cos, sin);
			code.sub(sin, regCache->m_vertexOneConst, cos2);
			code.div(sin, sin, regCache->m_vertexTwoConst);
			code.sqt(sin, sin);
			code.mul(R ^ Regs::xyz, sin, nrmVel);
			code.mul(R_rev ^ Regs::xyz, sin, nrmVel);
			code.neg(R_rev ^ Regs::xyz, R_rev);
			code.crs(nrmVel ^ Regs::xyz, R, rotationReg);
			code.mul(xAxis ^ Regs::xyz, cos, rotationReg);
			code.add(nrmVel ^ Regs::xyz, nrmVel, xAxis);
			code.dp3(xAxis ^ Regs::w, R, rotationReg);
			code.neg(cos2, xAxis ^ Regs::w);
			code.crs(R ^ Regs::xyz, nrmVel, R_rev);
			code.mul(xAxis, nrmVel, cos);
			code.add(R ^ Regs::xyz, R, xAxis);
			code.mul(xAxis ^ Regs::xyz, cos2, R_rev);
			code.add(rotationReg ^ Regs::xyz, R, xAxis);
		}
	}
}

AnimationStateBase* ParticleRotateToHeadingNode::createAnimationState(IAnimator* animator)
{
	return new ParticleRotateToHeadingState(animator, this);
}

void ParticleRotateToHeadingNode::processAnimationSetting(ParticleAnimationSet* particleAnimationSet)
{
	particleAnimationSet->m_needVelocity = true;
}