#include "ParticleFollowState.h"
#include "ParticleFollowNode.h"
#include "ObjectContainer3D.h"
#include "AnimationSubGeometry.h"
#include "AnimationRegisterCache.h"
#include "IContext.h"
#include "ParticleAnimationData.h"

USING_AWAY_NAMESPACE

ParticleFollowState::ParticleFollowState(IAnimator* animator, ParticleFollowNode* particleFollowNode) : ParticleStateBase(animator, particleFollowNode, true)
{
	m_initFlag = false;
	m_followTarget = nullptr;
}

void ParticleFollowState::setRenderState(IContext* context, IRenderable* renderable, AnimationSubGeometry* subGeometry, AnimationRegisterCache* regCache, Camera3D* camera)
{
	ParticleFollowNode* particleFollowNode = static_cast<ParticleFollowNode*>(m_animationNode);
	if (m_followTarget)
	{
		if (particleFollowNode->m_usesPosition)
		{
			Vector3D* scenePosition = m_followTarget->getScenePosition();
			m_targetPos.m_x = scenePosition->m_x;
			m_targetPos.m_y = scenePosition->m_y;
			m_targetPos.m_z = scenePosition->m_z;
		}
		if (particleFollowNode->m_usesRotation)
		{
			m_targetEuler.m_x = m_followTarget->getRotationX();
			m_targetEuler.m_y = m_followTarget->getRotationY();
			m_targetEuler.m_z = m_followTarget->getRotationZ();
			m_targetEuler.scaleBy(MathConsts::DEGREES_TO_RADIANS);
		}
	}

	if (!m_initFlag)
	{
		m_initFlag = true;
		subGeometry->m_previousTime = -MathConsts::Infinity;
	}

	float currentTime = m_time;
	float previousTime = subGeometry->m_previousTime;
	float deltaTime = currentTime - previousTime;
	bool needProcess = previousTime != currentTime;

	if (particleFollowNode->m_usesPosition && particleFollowNode->m_usesRotation)
	{
		if (needProcess)
			processPositionAndRotation(currentTime, deltaTime, subGeometry);

		subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleFollowNode::FOLLOW_POSITION_INDEX), particleFollowNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
		subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleFollowNode::FOLLOW_ROTATION_INDEX), particleFollowNode->m_dataOffset + 3, context, VertexBufferFormat::FLOAT_3);
	}
	else if (particleFollowNode->m_usesPosition)
	{
		if (needProcess)
			processPosition(currentTime, deltaTime, subGeometry);

		subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleFollowNode::FOLLOW_POSITION_INDEX), particleFollowNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
	}
	else if (particleFollowNode->m_usesRotation)
	{
		if (needProcess)
			precessRotation(currentTime, deltaTime, subGeometry);

		subGeometry->activateVertexBuffer(regCache->getRegisterIndex(m_animationNode, ParticleFollowNode::FOLLOW_ROTATION_INDEX), particleFollowNode->m_dataOffset, context, VertexBufferFormat::FLOAT_3);
	}

	m_prePos.copyFrom(m_targetPos);
	m_preEuler.copyFrom(m_targetEuler);
	subGeometry->m_previousTime = currentTime;
}

void ParticleFollowState::processPosition(float currentTime, float deltaTime, AnimationSubGeometry* subGeometry)
{
	Vector3D posVelocity, interpolatedPos, temp;
	ParticleFollowNode* particleFollowNode = static_cast<ParticleFollowNode*>(m_animationNode);
	if (particleFollowNode->m_smooth)
	{
		m_prePos.subtract(m_targetPos, posVelocity);
		posVelocity.scaleBy(1 / deltaTime);
	}
	else
		interpolatedPos.copyFrom(m_targetPos);

	bool changed = false;
	float* vertexData = subGeometry->getVertexData();
	int totalLenOfOneVertex = subGeometry->getTotalLenOfOneVertex();
	for (auto data : subGeometry->m_animationParticles)
	{
		float k = (currentTime - data->m_startTime) / data->m_totalTime;
		float t = (k - std::floor(k)) * data->m_totalTime;
		if (t - deltaTime <= 0)
		{
			if (particleFollowNode->m_smooth)
			{
				temp.copyFrom(posVelocity);
				temp.scaleBy(t);
				m_targetPos.add(temp, interpolatedPos);
			}

			int inc = data->m_startVertexIndex * totalLenOfOneVertex + particleFollowNode->m_dataOffset;
			if (vertexData[inc] != interpolatedPos.m_x || vertexData[inc + 1] != interpolatedPos.m_y || vertexData[inc + 2] != interpolatedPos.m_z)
			{
				for (int i = 0; i < data->m_numVertices; i++)
				{
					vertexData[inc++] = interpolatedPos.m_x;
					vertexData[inc++] = interpolatedPos.m_y;
					vertexData[inc++] = interpolatedPos.m_z;
				}
				changed = true;
			}
		}
	}

	if (changed)
		subGeometry->invalidateBuffer();
}

void ParticleFollowState::precessRotation(float currentTime, float deltaTime, AnimationSubGeometry* subGeometry)
{
	Vector3D rotVelocity, interpolatedRot, temp;
	ParticleFollowNode* particleFollowNode = static_cast<ParticleFollowNode*>(m_animationNode);
	if (particleFollowNode->m_smooth)
	{
		m_preEuler.subtract(m_targetEuler, rotVelocity);
		rotVelocity.scaleBy(1 / deltaTime);
	}
	else
		interpolatedRot.copyFrom(m_targetEuler);

	bool changed = false;
	float* vertexData = subGeometry->getVertexData();
	int totalLenOfOneVertex = subGeometry->getTotalLenOfOneVertex();
	for (auto data : subGeometry->m_animationParticles)
	{
		float k = (currentTime - data->m_startTime) / data->m_totalTime;
		float t = (k - std::floor(k)) * data->m_totalTime;
		if (t - deltaTime <= 0)
		{
			if (particleFollowNode->m_smooth)
			{
				temp.copyFrom(rotVelocity);
				temp.scaleBy(t);
				m_targetEuler.add(temp, interpolatedRot);
			}

			int inc = data->m_startVertexIndex * totalLenOfOneVertex + particleFollowNode->m_dataOffset;
			if (vertexData[inc] != interpolatedRot.m_x || vertexData[inc + 1] != interpolatedRot.m_y || vertexData[inc + 2] != interpolatedRot.m_z)
			{
				for (int i = 0; i < data->m_numVertices; i++)
				{
					vertexData[inc++] = interpolatedRot.m_x;
					vertexData[inc++] = interpolatedRot.m_y;
					vertexData[inc++] = interpolatedRot.m_z;
				}
				changed = true;
			}
		}
	}

	if (changed)
		subGeometry->invalidateBuffer();
}

void ParticleFollowState::processPositionAndRotation(float currentTime, float deltaTime, AnimationSubGeometry* subGeometry)
{
	Vector3D posVelocity, rotVelocity, interpolatedPos, interpolatedRot, temp;
	ParticleFollowNode* particleFollowNode = static_cast<ParticleFollowNode*>(m_animationNode);
	if (particleFollowNode->m_smooth)
	{
		m_prePos.subtract(m_targetPos, posVelocity);
		posVelocity.scaleBy(1 / deltaTime);
		m_preEuler.subtract(m_targetEuler, rotVelocity);
		rotVelocity.scaleBy(1 / deltaTime);
	}
	else
	{
		interpolatedPos.copyFrom(m_targetPos);
		interpolatedRot.copyFrom(m_targetEuler);
	}

	bool changed = false;
	float* vertexData = subGeometry->getVertexData();
	int totalLenOfOneVertex = subGeometry->getTotalLenOfOneVertex();
	for (auto data : subGeometry->m_animationParticles)
	{
		float k = (currentTime - data->m_startTime) / data->m_totalTime;
		float t = (k - std::floor(k)) * data->m_totalTime;
		if (t - deltaTime <= 0)
		{
			if (particleFollowNode->m_smooth)
			{
				temp.copyFrom(posVelocity);
				temp.scaleBy(t);
				m_targetPos.add(temp, interpolatedPos);
				temp.copyFrom(rotVelocity);
				temp.scaleBy(t);
				m_targetEuler.add(temp, interpolatedRot);
			}

			int inc = data->m_startVertexIndex * totalLenOfOneVertex + particleFollowNode->m_dataOffset;
			if (vertexData[inc] != interpolatedPos.m_x || vertexData[inc + 1] != interpolatedPos.m_y || vertexData[inc + 2] != interpolatedPos.m_z ||
				vertexData[inc + 3] != interpolatedRot.m_x || vertexData[inc + 4] != interpolatedRot.m_y || vertexData[inc + 5] != interpolatedRot.m_z)
			{
				for (int i = 0; i < data->m_numVertices; i++)
				{
					vertexData[inc++] = interpolatedPos.m_x;
					vertexData[inc++] = interpolatedPos.m_y;
					vertexData[inc++] = interpolatedPos.m_z;
					vertexData[inc++] = interpolatedRot.m_x;
					vertexData[inc++] = interpolatedRot.m_y;
					vertexData[inc++] = interpolatedRot.m_z;
				}
				changed = true;
			}
		}
	}

	if (changed)
		subGeometry->invalidateBuffer();
}