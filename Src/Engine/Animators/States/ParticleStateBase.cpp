#include "ParticleStateBase.h"
#include "ParticleAnimator.h"
#include "ParticleNodeBase.h"
#include "AnimationSubGeometry.h"
#include "ParticleAnimationData.h"

USING_AWAY_NAMESPACE

ParticleStateBase::ParticleStateBase(IAnimator* animator, ParticleNodeBase* particleNode, bool needUpdateTime) : AnimationStateBase(animator, particleNode)
{
	m_needUpdateTime = needUpdateTime;
}

void ParticleStateBase::updateDynamicProperties(AnimationSubGeometry* subGeometry)
{
	m_dynamicPropertiesDirty[subGeometry] = true;

	ParticleAnimationData* animationParticle;
	int numParticles = m_dynamicProperties.size();
	std::vector<ParticleAnimationData*>& animationParticles = subGeometry->m_animationParticles;
	int totalLenOfOneVertex = subGeometry->getTotalLenOfOneVertex();
	int dataLength = static_cast<ParticleNodeBase*>(m_animationNode)->getDataLength();
	int dataOffset = static_cast<ParticleNodeBase*>(m_animationNode)->m_dataOffset;
	int vertexLength, startingOffset, vertexOffset;
	float* vertexData = subGeometry->getVertexData();

	// loop through all particles
	for (int i = 0, j = 0, k; i < numParticles; i++)
	{
		// loop through each particle data for the current particle
		for (; j < numParticles && (animationParticle = animationParticles[j])->m_index == i; j++)
		{
			Vector3D& data = m_dynamicProperties[i];
			vertexLength = animationParticle->m_numVertices * totalLenOfOneVertex;
			startingOffset = animationParticle->m_startVertexIndex * totalLenOfOneVertex + dataOffset;

			// loop through each vertex in the particle data
			for (k = 0; k < vertexLength; k += totalLenOfOneVertex)
			{
				vertexOffset = startingOffset + k;

				// loop through all vertex data for the current particle data
				for (k = 0; k < vertexLength; k += totalLenOfOneVertex)
				{
					vertexOffset = startingOffset + k;
					vertexData[vertexOffset++] = data.m_x;
					vertexData[vertexOffset++] = data.m_y;
					vertexData[vertexOffset++] = data.m_z;

					if (dataLength == 4)
						vertexData[vertexOffset++] = data.m_w;
				}
			}
		}
	}

	subGeometry->invalidateBuffer();
}