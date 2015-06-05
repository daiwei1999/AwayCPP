#include "ParticleAnimator.h"
#include "ParticleAnimationSet.h"
#include "ParticleNodeBase.h"
#include "ParticleStateBase.h"
#include "AnimationRegisterCache.h"
#include "SubMesh.h"
#include "IContext.h"
#include "AnimationSubGeometry.h"

USING_AWAY_NAMESPACE

ParticleAnimator::ParticleAnimator(ParticleAnimationSet* particleAnimationSet) : AnimatorBase(particleAnimationSet)
{
	m_totalLenOfOneVertex = 0;

	ParticleStateBase* state;
	for (ParticleNodeBase* node : static_cast<ParticleAnimationSet*>(m_animationSet)->getParticleNodes())
	{
		state = static_cast<ParticleStateBase*>(getAnimationState(node));
		if (node->getMode() == ParticlePropertiesMode::LOCAL_DYNAMIC)
		{
			m_animatorParticleStates.push_back(state);
			node->m_dataOffset = m_totalLenOfOneVertex;
			m_totalLenOfOneVertex += node->getDataLength();
		}
		else
			m_animationParticleStates.push_back(state);

		if (state->needUpdateTime())
			m_timeParticleStates.push_back(state);
	}
}

ParticleAnimator::~ParticleAnimator()
{

}

void ParticleAnimator::start()
{
	AnimatorBase::start();

	for (ParticleStateBase* state : m_timeParticleStates)
		state->offset(m_absoluteTime);
}

void ParticleAnimator::resetTime(int offset)
{
	for (ParticleStateBase* state : m_timeParticleStates)
		state->offset(m_absoluteTime + offset);

	update(getTime());
}

void ParticleAnimator::setRenderState(IContext* context, IRenderable* renderable, int vertexConstantOffset, int vertexStreamOffset, Camera3D* camera)
{
	ParticleAnimationSet* particleAnimationSet = static_cast<ParticleAnimationSet*>(m_animationSet);
	AnimationRegisterCache* regCache = particleAnimationSet->m_animationRegisterCache;
	SubMesh* subMesh = static_cast<SubMesh*>(renderable);

	// process animation sub geometries
	if (!subMesh->m_animationSubGeometry)
		particleAnimationSet->generateAnimationSubGeometries(subMesh->getParentMesh());

	for (ParticleStateBase* state : m_animationParticleStates)
		state->setRenderState(context, renderable, subMesh->m_animationSubGeometry, regCache, camera);

	// process animator sub geometries
	if (!subMesh->m_animatorSubGeometry && m_animatorParticleStates.size() > 0)
		generateAnimatorSubGeometry(subMesh);

	for (ParticleStateBase* state : m_animatorParticleStates)
		state->setRenderState(context, renderable, subMesh->m_animatorSubGeometry, regCache, camera);

	context->setProgramConstantsFromVector(ProgramType::VERTEX, regCache->getVertexConstantOffset(), regCache->m_vertexConstantData.data(), regCache->getVertexConstantCount());

	if (regCache->getFragmentConstantCount() > 0)
		context->setProgramConstantsFromVector(ProgramType::FRAGMENT, regCache->getFragmentConstantOffset(), regCache->m_fragmentConstantData.data(), regCache->getFragmentConstantCount());
}

void ParticleAnimator::testGPUCompatibility(MaterialPassBase* pass)
{

}

ParticleAnimator* ParticleAnimator::clone()
{
	return new ParticleAnimator(static_cast<ParticleAnimationSet*>(m_animationSet));
}

void ParticleAnimator::updateDeltaTime(float dt)
{
	m_absoluteTime += dt;

	for (ParticleStateBase* state : m_timeParticleStates)
		state->update(m_absoluteTime);
}

void ParticleAnimator::generateAnimatorSubGeometry(SubMesh* subMesh)
{
	ISubGeometry* subGeometry = subMesh->getSubGeometry();
	AnimationSubGeometry* animatorSubGeometry = new AnimationSubGeometry();
	m_animatorSubGeometries[subGeometry] = subMesh->m_animatorSubGeometry = animatorSubGeometry;

	// create the vertexData vector that will be used for local state data
	animatorSubGeometry->createVertexData(subGeometry->getVertexCount(), m_totalLenOfOneVertex);

	// pass the particles data to the animator subGeometry
	animatorSubGeometry->m_animationParticles = subMesh->m_animationSubGeometry->m_animationParticles;
}