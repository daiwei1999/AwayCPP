#include "ParticleAnimationSet.h"
#include "ParticleTimeNode.h"
#include "MaterialPassBase.h"
#include "AnimationRegisterCache.h"
#include "IContext.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "Mesh.h"
#include "SubMesh.h"
#include "ParticleGeometry.h"
#include "AnimationSubGeometry.h"
#include "ParticleAnimationData.h"

USING_AWAY_NAMESPACE

const int ParticleAnimationSet::POST_PRIORITY = 9;
const int ParticleAnimationSet::COLOR_PRIORITY = 18;

ParticleAnimationSet::ParticleAnimationSet(bool usesDuration, bool usesLooping, bool usesDelay, bool usesRibbon)
{
	m_hasUVNode = false;
	m_needVelocity = false;
	m_hasBillboard = false;
	m_hasColorMulNode = false;
	m_hasColorAddNode = false;
	m_animationRegisterCache = nullptr;
	m_totalLenOfOneVertex = 0;
	addAnimation(new ParticleTimeNode(usesDuration, usesLooping, usesDelay, usesRibbon));
}

void ParticleAnimationSet::generateAnimationSubGeometries(Mesh* mesh)
{
	ISubGeometry* subGeometry;
	bool newAnimationSubGeometry = false;
	for (SubMesh* subMesh : mesh->getSubMeshes())
	{
		subGeometry = subMesh->getSubGeometry();
		if (mesh->getShareAnimationGeometry())
		{
			if (m_animationSubGeometries.count(subGeometry) > 0)
			{
				subMesh->m_animationSubGeometry = m_animationSubGeometries[subGeometry];
				continue;
			}
		}

		subMesh->m_animationSubGeometry = new AnimationSubGeometry();
		if (mesh->getShareAnimationGeometry())
			m_animationSubGeometries[subGeometry] = subMesh->m_animationSubGeometry;

		newAnimationSubGeometry = true;

		// create the vertexData vector that will be used for local node data
		subMesh->m_animationSubGeometry->createVertexData(subGeometry->getVertexCount(), m_totalLenOfOneVertex);
	}

	if (!newAnimationSubGeometry)
		return;

	ParticleGeometry* geometry = static_cast<ParticleGeometry*>(mesh->getGeometry());
	std::vector<ParticleData*>& particles = geometry->m_particles;
	int particlesLength = particles.size();
	int numParticles = geometry->m_numParticles;

	ParticleProperties particleProperties;
	particleProperties.m_total = numParticles;
	particleProperties.m_startTime = 0;
	particleProperties.m_duration = 1000;
	particleProperties.m_delay = 0.1f;
	
	ParticleData* particle;
	float *vertexData, *oneData;
	AnimationSubGeometry* animationSubGeometry = nullptr;
	int numVertices, vertexLength, startingOffset, oneDataLen, oneDataOffset, counterForVertex, vertexOffset, counterForOneData;
	
	for (int i = 0, j = 0; i < numParticles; i++)
	{
		particleProperties.m_index = i;

		// call the init function on the particle parameters
		m_initParticleFunc(particleProperties);

		// create the next set of node properties for the particle
		for (ParticleNodeBase* localNode : m_localStaticNodes)
			localNode->generatePropertyOfOneParticle(particleProperties);

		// loop through all particle data for the current particle
		for (; j < particlesLength && (particle = particles[j])->m_particleIndex == i; j++)
		{
			// find the target animationSubGeometry
			for (SubMesh* subMesh : mesh->getSubMeshes())
			{
				if (subMesh->getSubGeometry() == particle->m_subGeometry)
				{
					animationSubGeometry = subMesh->m_animationSubGeometry;
					break;
				}
			}

			numVertices = particle->m_numVertices;
			vertexData = animationSubGeometry->getVertexData();
			vertexLength = numVertices * m_totalLenOfOneVertex;
			startingOffset = animationSubGeometry->m_numProcessedVertices * m_totalLenOfOneVertex;

			// loop through each static local node in the animation set
			for (ParticleNodeBase* localNode : m_localStaticNodes)
			{
				oneData = localNode->getOneData();
				oneDataLen = localNode->getDataLength();
				oneDataOffset = startingOffset + localNode->m_dataOffset;

				// loop through each vertex set in the vertex data
				for (counterForVertex = 0; counterForVertex < vertexLength; counterForVertex += m_totalLenOfOneVertex)
				{
					vertexOffset = oneDataOffset + counterForVertex;

					// add the data for the local node to the vertex data
					for (counterForOneData = 0; counterForOneData < oneDataLen; counterForOneData++)
						vertexData[vertexOffset + counterForOneData] = oneData[counterForOneData];
				}
			}

			// store particle properties if they need to be retrieved for dynamic local nodes
			if (m_localDynamicNodes.size() > 0)
				animationSubGeometry->m_animationParticles.push_back(new ParticleAnimationData(i, particleProperties.m_startTime, particleProperties.m_duration, particleProperties.m_delay, particle));

			animationSubGeometry->m_numProcessedVertices += numVertices;
		}
	}
}

void ParticleAnimationSet::addAnimation(AnimationNodeBase* node)
{
	ParticleNodeBase* particleNode = static_cast<ParticleNodeBase*>(node);
	particleNode->processAnimationSetting(this);

	ParticlePropertiesMode mode = particleNode->getMode();
	if (mode == ParticlePropertiesMode::LOCAL_STATIC)
	{
		particleNode->m_dataOffset = m_totalLenOfOneVertex;
		m_totalLenOfOneVertex += particleNode->getDataLength();
		m_localStaticNodes.push_back(particleNode);
	}
	else if (mode == ParticlePropertiesMode::LOCAL_DYNAMIC)
	{
		m_localDynamicNodes.push_back(particleNode);
	}
	
	// keep m_particleNodes in priority ascending order
	int len = m_particleNodes.size();
	if (len > 0)
	{
		int i, priority = particleNode->getPriority();
		for (i = len - 1; i >= 0; i--)
		{
			if (m_particleNodes[i]->getPriority() <= priority)
				break;
		}
		m_particleNodes.insert(m_particleNodes.begin() + i + 1, particleNode);
	}
	else
	{
		m_particleNodes.push_back(particleNode);
	}

	AnimationSetBase::addAnimation(node);
}

void ParticleAnimationSet::getAGALVertexCode(ShaderChunk& code, MaterialPassBase* pass, std::vector<unsigned int>& sourceRegisters, std::vector<unsigned int>& targetRegisters, unsigned int uvSource, unsigned int uvTarget)
{
	if (!pass->m_animationRegisterCache)
		pass->m_animationRegisterCache = new AnimationRegisterCache();

	// reset animationRegisterCache
	m_animationRegisterCache = pass->m_animationRegisterCache;
	m_animationRegisterCache->setVertexConstantOffset(pass->getUsedVertexConstantCount());
	m_animationRegisterCache->setVertexAttributesOffset(pass->getUsedStreamCount());
	m_animationRegisterCache->setVaryingsOffset(pass->getUsedVaryingCount());
	m_animationRegisterCache->setFragmentConstantOffset(pass->getUsedFragmentConstantCount());
	m_animationRegisterCache->m_hasUVNode = m_hasUVNode;
	m_animationRegisterCache->m_needVelocity = m_needVelocity;
	m_animationRegisterCache->m_hasBillboard = m_hasBillboard;
	m_animationRegisterCache->m_sourceRegisters = sourceRegisters;
	m_animationRegisterCache->m_targetRegisters = targetRegisters;
	m_animationRegisterCache->m_needFragmentAnimation = pass->needFragmentAnimation();
	m_animationRegisterCache->m_needUVAnimation = pass->needUVAnimation();
	m_animationRegisterCache->m_hasColorAddNode = m_hasColorAddNode;
	m_animationRegisterCache->m_hasColorMulNode = m_hasColorMulNode;
	m_animationRegisterCache->reset();
	m_animationRegisterCache->setUVSourceAndTarget(uvSource, uvTarget);

	m_animationRegisterCache->getInitCode(code);

	for (ParticleNodeBase* node : m_particleNodes)
	{
		if (node->getPriority() < POST_PRIORITY)
			node->getAGALVertexCode(code, m_animationRegisterCache);
	}

	m_animationRegisterCache->getCombinationCode(code);

	for (ParticleNodeBase* node : m_particleNodes)
	{
		if (node->getPriority() >= POST_PRIORITY && node->getPriority() < COLOR_PRIORITY)
			node->getAGALVertexCode(code, m_animationRegisterCache);
	}

	m_animationRegisterCache->initColorRegisters(code);

	for (ParticleNodeBase* node : m_particleNodes)
	{
		if (node->getPriority() >= COLOR_PRIORITY)
			node->getAGALVertexCode(code, m_animationRegisterCache);
	}

	m_animationRegisterCache->getColorPassCode(code);
}

void ParticleAnimationSet::getAGALFragmentCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int shadedTarget)
{
	m_animationRegisterCache->getColorCombinationCode(code, shadedTarget);
	for (ParticleNodeBase* node : m_particleNodes)
		node->getAGALFragmentCode(code, m_animationRegisterCache);
}

void ParticleAnimationSet::getAGALUVCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int uvSource, unsigned int uvTarget)
{
	if (m_hasUVNode)
	{
		code.mov(m_animationRegisterCache->m_uvTarget ^ Regs::xy, m_animationRegisterCache->m_uvAttribute);

		for (ParticleNodeBase* node : m_particleNodes)
			node->getAGALUVCode(code, m_animationRegisterCache);

		code.mov(m_animationRegisterCache->m_uvVaring, m_animationRegisterCache->m_uvTarget ^ Regs::xy);
	}
	else
		code.mov(uvTarget, uvSource);
}

void ParticleAnimationSet::doneAGALCode(MaterialPassBase* pass)
{
	m_animationRegisterCache->setDataLength();
	m_animationRegisterCache->setVertexConst(m_animationRegisterCache->m_vertexZeroConst & 0xFF, 0, 1, 2, 0);
}

void ParticleAnimationSet::activate(IContext* context, MaterialPassBase* pass)
{
	m_animationRegisterCache = pass->m_animationRegisterCache;
}

void ParticleAnimationSet::deactivate(IContext* context, MaterialPassBase* pass)
{
	int offset = m_animationRegisterCache->getVertexAttributesOffset();
	int used = m_animationRegisterCache->getUsedStreamCount();
	for (int i = offset; i < used; i++)
		context->setVertexBufferAt(i, nullptr);
}