#include "ParticleOrbitNode.h"
#include "AnimationRegisterCache.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "ParticleOrbitState.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

const int ParticleOrbitNode::ORBIT_INDEX = 0;
const int ParticleOrbitNode::EULERS_INDEX = 1;
const std::string ParticleOrbitNode::ORBIT_VECTOR3D = "OrbitVector3D";

ParticleOrbitNode::ParticleOrbitNode(ParticlePropertiesMode mode, bool usesEulers, bool usesCycle, bool usesPhase,
									 float radius, float cycleDuration, float cyclePhase, Vector3D* eulers) : ParticleNodeBase("ParticleOrbit", mode, usesPhase ? 4 : 3)
{
	m_usesEulers = usesEulers;
	m_usesCycle = usesCycle;
	m_usesPhase = usesPhase;

	m_radius = radius;
	m_cycleDuration = cycleDuration;
	m_cyclePhase = cyclePhase;
	m_eulers = eulers;
}

void ParticleOrbitNode::getAGALVertexCode(ShaderChunk& code, AnimationRegisterCache* regCache)
{
	unsigned int orbitReg = (m_mode == ParticlePropertiesMode::GLOBAL) ? regCache->getFreeVertexConstant() : regCache->getFreeVertexAttribute();
	regCache->setRegisterIndex(this, ORBIT_INDEX, REGISTER_INDEX(orbitReg));

	unsigned int eulersMatrixReg = regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->setRegisterIndex(this, EULERS_INDEX, REGISTER_INDEX(eulersMatrixReg));

	unsigned int temp1 = regCache->getFreeVertexVectorTemp();
	regCache->addVertexTempUsages(temp1, 1);
	unsigned int temp2 = regCache->getFreeVertexVectorTemp();
	regCache->removeVertexTempUsage(temp1);

	if (m_usesCycle)
	{
		code.mul(temp2 ^ Regs::z, regCache->m_vertexTime, orbitReg ^ Regs::y);
		if (m_usesPhase)
			code.add(temp2 ^ Regs::z, temp2 ^ Regs::z, orbitReg ^ Regs::w);
	}
	else
		code.mul(temp2 ^ Regs::z, regCache->m_vertexLife, orbitReg ^ Regs::y);

	code.cos(temp2 ^ Regs::x, temp2 ^ Regs::z);
	code.sin(temp2 ^ Regs::y, temp2 ^ Regs::z);
	code.mul(temp1 ^ Regs::x, temp2 ^ Regs::x, orbitReg ^ Regs::x);
	code.mul(temp1 ^ Regs::y, temp2 ^ Regs::y, orbitReg ^ Regs::x);
	code.mov(temp1 ^ Regs::wz, regCache->m_vertexZeroConst);

	if (m_usesEulers)
		code.m44(temp1, temp1, eulersMatrixReg);

	code.add(regCache->m_positionTarget ^ Regs::xyz, regCache->m_positionTarget, temp1);

	if (regCache->m_needVelocity)
	{
		code.neg(temp1 ^ Regs::x, temp2 ^ Regs::y);
		code.mov(temp1 ^ Regs::y, temp2 ^ Regs::x);
		code.mov(temp1 ^ Regs::zw, regCache->m_vertexZeroConst);

		if (m_usesEulers)
			code.m44(temp1, temp1, eulersMatrixReg);

		code.mul(temp1, temp1, orbitReg ^ Regs::z);
		code.div(temp1, temp1, orbitReg ^ Regs::y);

		if (!m_usesCycle)
			code.div(temp1, temp1, regCache->m_vertexLife);

		code.add(regCache->m_velocityTarget ^ Regs::xyz, regCache->m_velocityTarget, temp1);
	}
}

AnimationStateBase* ParticleOrbitNode::createAnimationState(IAnimator* animator)
{
	return new ParticleOrbitState(animator, this);
}

void ParticleOrbitNode::generatePropertyOfOneParticle(ParticleProperties& param)
{
	Vector3D& orbit = param[ORBIT_VECTOR3D];
	m_oneData[0] = orbit.m_x;
	m_oneData[1] = MathConsts::TWO_PI / (!m_usesCycle ? 1 : orbit.m_y);
	m_oneData[2] = orbit.m_x * MathConsts::TWO_PI;
	if (m_usesPhase)
		m_oneData[3] = orbit.m_z * MathConsts::DEGREES_TO_RADIANS;
}