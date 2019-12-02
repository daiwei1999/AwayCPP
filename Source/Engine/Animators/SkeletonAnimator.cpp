#include "SkeletonAnimator.h"
#include "SkeletonAnimationSet.h"
#include "Skeleton.h"
#include "SkeletonClipState.h"
#include "SubMesh.h"
#include "SkinnedSubGeometry.h"
#include "MaterialPassBase.h"
#include "IContext.h"
#include "JointTag.h"

USING_AWAY_NAMESPACE

SubGeomAnimationState::SubGeomAnimationState(SkinnedSubGeometry* subGeom)
{
	int numFloats = subGeom->getVertexStride() * subGeom->getVertexCount();
	m_animatedVertexData = new float[numFloats];
	std::memcpy(m_animatedVertexData, subGeom->getVertexData(), numFloats * sizeof(float));
}

SubGeomAnimationState::~SubGeomAnimationState()
{
	delete[] m_animatedVertexData;
}

SkeletonAnimator::SkeletonAnimator(SkeletonAnimationSet* animationSet, Skeleton* skeleton, bool forceCPU) : AnimatorBase(animationSet)
{
	m_skeleton = skeleton;
	m_forceCPU = forceCPU;
	m_globalPropertiesDirty = false;
	m_jointsPerVertex = animationSet->getJointsPerVertex();

	std::vector<SkeletonJoint*>& joints = m_skeleton->m_joints;
	int numTotalJoints = joints.size();
	for (int i = 0; i < numTotalJoints; i++)
	{
		float(&raw)[12] = joints[i]->m_inverseBindPose;
		Matrix3D bindPose(raw[0], raw[1], raw[2], 0, raw[3], raw[4], raw[5], 0, raw[6], raw[7], raw[8], 0, raw[9], raw[10], raw[11], 1);
		bindPose.invert();
		m_globalMatrices.push_back(bindPose);
	}

	int numSkinningJoints = m_skeleton->m_numSkinningJoints;
	m_vertexData.resize(numSkinningJoints * 12);
	for (int i = 0, j = 0; i < numSkinningJoints; i++)
	{
		m_vertexData[j++] = 1;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 1;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 0;
		m_vertexData[j++] = 1;
		m_vertexData[j++] = 0;
	}
}

SkeletonAnimator::~SkeletonAnimator()
{
	for (auto state : m_subGeomAnimStates)
		delete state.second;
}

std::vector<Matrix3D>& SkeletonAnimator::getGlobalMatrices()
{
	if (m_globalPropertiesDirty)
		updateGlobalProperties();

	return m_globalMatrices;
}

void SkeletonAnimator::play(const std::string& name, float offset)
{
	if (!m_animationSet->hasAnimation(name))
		return;

	if (m_activeAnimationName != name)
	{
		m_activeAnimationName = name;

		m_activeNode = m_animationSet->getAnimation(name);
		m_activeState = getAnimationState(m_activeNode);
	}

	start();

	// apply a time offset if specified
	reset(name, offset);
}

void SkeletonAnimator::setRenderState(IContext* context, IRenderable* renderable, int vertexConstantOffset, int vertexStreamOffset, Camera3D* camera)
{
	if (m_globalPropertiesDirty)
		updateGlobalProperties();

	SkinnedSubGeometry* skinnedGeom = static_cast<SkinnedSubGeometry*>(static_cast<SubMesh*>(renderable)->getSubGeometry());

	if (m_animationSet->usesCPU())
	{
		SubGeomAnimationState* subGeomAnimState = m_subGeomAnimStates[skinnedGeom];
		if (!subGeomAnimState)
		{
			subGeomAnimState = new SubGeomAnimationState(skinnedGeom);
			m_subGeomAnimStates[skinnedGeom] = subGeomAnimState;
		}
		if (subGeomAnimState->m_dirty)
		{
			morphGeometry(subGeomAnimState->m_animatedVertexData, skinnedGeom);
			subGeomAnimState->m_dirty = false;
		}
		skinnedGeom->updateAnimatedData(subGeomAnimState->m_animatedVertexData);
	}
	else
	{
		context->setProgramConstantsFromVector(ProgramType::VERTEX, vertexConstantOffset, m_vertexData.data(), m_skeleton->m_numSkinningJoints * 3);
		skinnedGeom->activateJointIndexBuffer(vertexStreamOffset, context);
		skinnedGeom->activateJointWeightsBuffer(vertexStreamOffset + 1, context);
	}
}

void SkeletonAnimator::testGPUCompatibility(MaterialPassBase* pass)
{
	if (m_forceCPU || m_jointsPerVertex > 4 || pass->getUsedVertexConstantCount() + m_skeleton->m_numSkinningJoints * 3 > 128)
		m_animationSet->cancelGPUCompatibility();
}

SkeletonAnimator* SkeletonAnimator::clone()
{
	return new SkeletonAnimator(static_cast<SkeletonAnimationSet*>(m_animationSet), m_skeleton, m_forceCPU);
}

void SkeletonAnimator::updateDeltaTime(float dt)
{
	AnimatorBase::updateDeltaTime(dt);

	// invalidate pose matrices
	m_globalPropertiesDirty = true;

	for (auto state : m_subGeomAnimStates)
		state.second->m_dirty = true;

	// invalidate transform of jointTags
	for (auto owner : m_owners)
	{
		int count = owner->getJointTagCount();
		for (int i = 0; i < count; i++)
			owner->getJointTagAt(i)->invalidateTransform();
	}
}

void SkeletonAnimator::updateGlobalProperties()
{
	m_globalPropertiesDirty = false;

	// get global pose
	SkeletonPose& sourcePose = static_cast<SkeletonClipState*>(m_activeState)->getSkeletonPose(m_skeleton);

	// convert local hierarchical skeleton pose to global pose
	Vector3D rotation;
	std::vector<int>& indexTable = m_skeleton->m_indexTable;
	std::vector<JointPose>& localPoses = sourcePose.m_jointPoses;
	std::vector<SkeletonJoint*>& joints = m_skeleton->m_joints;
	int numTotalJoints = joints.size();
	for (int i = 0; i < numTotalJoints; i++)
	{
		int index = indexTable[i];
		JointPose& pose = localPoses[index];
		Matrix3D& globalMatrix = m_globalMatrices[index];

		// convert translation and rotation to matrix
		std::memcpy(&rotation, &pose.m_orientation, 16);
		globalMatrix.recompose(pose.m_translation, rotation, pose.m_scaling, Orientation3D::QUATERNION);

		// append parent pose
		int parentIndex = joints[index]->m_parentIndex;
		if (parentIndex >= 0)
			globalMatrix.append(m_globalMatrices[parentIndex]);
	}

	int numSkinningJoints = m_skeleton->m_numSkinningJoints;
	for (int i = 0, j = 0; i < numSkinningJoints; i++)
	{
		const float* rawData = m_globalMatrices[i].m_rawData;
		float n11 = rawData[0];
		float n12 = rawData[4];
		float n13 = rawData[8];
		float n21 = rawData[1];
		float n22 = rawData[5];
		float n23 = rawData[9];
		float n31 = rawData[2];
		float n32 = rawData[6];
		float n33 = rawData[10];

		// prepend inverse bind pose
		float(&raw)[12] = joints[i]->m_inverseBindPose;
		float m11 = raw[0];
		float m12 = raw[3];
		float m13 = raw[6];
		float m14 = raw[9];
		float m21 = raw[1];
		float m22 = raw[4];
		float m23 = raw[7];
		float m24 = raw[10];
		float m31 = raw[2];
		float m32 = raw[5];
		float m33 = raw[8];
		float m34 = raw[11];

		m_vertexData[j++] = n11 * m11 + n12 * m21 + n13 * m31;
		m_vertexData[j++] = n11 * m12 + n12 * m22 + n13 * m32;
		m_vertexData[j++] = n11 * m13 + n12 * m23 + n13 * m33;
		m_vertexData[j++] = n11 * m14 + n12 * m24 + n13 * m34 + rawData[12];
		m_vertexData[j++] = n21 * m11 + n22 * m21 + n23 * m31;
		m_vertexData[j++] = n21 * m12 + n22 * m22 + n23 * m32;
		m_vertexData[j++] = n21 * m13 + n22 * m23 + n23 * m33;
		m_vertexData[j++] = n21 * m14 + n22 * m24 + n23 * m34 + rawData[13];
		m_vertexData[j++] = n31 * m11 + n32 * m21 + n33 * m31;
		m_vertexData[j++] = n31 * m12 + n32 * m22 + n33 * m32;
		m_vertexData[j++] = n31 * m13 + n32 * m23 + n33 * m33;
		m_vertexData[j++] = n31 * m14 + n32 * m24 + n33 * m34 + rawData[14];
	}
}

void SkeletonAnimator::morphGeometry(float* targetData, SkinnedSubGeometry* subGeom)
{
	float* vertexData = subGeom->getVertexData();
	float* jointWeights = subGeom->getJointWeightsData();
	unsigned char* jointIndices = subGeom->getJointIndexData();

	unsigned char k;
	float vx, vy, vz, nx, ny, nz, tx, ty, tz, weight;
	float m11, m12, m13, m21, m22, m23, m31, m32, m33;
	float vertX, vertY, vertZ, normX, normY, normZ, tangX, tangY, tangZ;
	int stride = subGeom->getVertexStride();
	int mtxOffset, len = stride * subGeom->getVertexCount();

	for (int i = 0, j = 0; i < len; i += stride)
	{
		vertX = vertexData[i];
		vertY = vertexData[i + 1];
		vertZ = vertexData[i + 2];
		normX = vertexData[i + 3];
		normY = vertexData[i + 4];
		normZ = vertexData[i + 5];
		tangX = vertexData[i + 6];
		tangY = vertexData[i + 7];
		tangZ = vertexData[i + 8];

		vx = vy = vz = nx = ny = nz = tx = ty = tz = 0;
		for (k = 0; k < m_jointsPerVertex; k++)
		{
			weight = jointWeights[j];
			if (weight > 0)
			{
				mtxOffset = jointIndices[j] << 2;
				m11 = m_vertexData[mtxOffset];
				m12 = m_vertexData[mtxOffset + 1];
				m13 = m_vertexData[mtxOffset + 2];
				m21 = m_vertexData[mtxOffset + 4];
				m22 = m_vertexData[mtxOffset + 5];
				m23 = m_vertexData[mtxOffset + 6];
				m31 = m_vertexData[mtxOffset + 8];
				m32 = m_vertexData[mtxOffset + 9];
				m33 = m_vertexData[mtxOffset + 10];
				vx += weight * (m11 * vertX + m12 * vertY + m13 * vertZ + m_vertexData[mtxOffset + 3]);
				vy += weight * (m21 * vertX + m22 * vertY + m23 * vertZ + m_vertexData[mtxOffset + 7]);
				vz += weight * (m31 * vertX + m32 * vertY + m33 * vertZ + m_vertexData[mtxOffset + 11]);
				nx += weight * (m11 * normX + m12 * normY + m13 * normZ);
				ny += weight * (m21 * normX + m22 * normY + m23 * normZ);
				nz += weight * (m31 * normX + m32 * normY + m33 * normZ);
				tx += weight * (m11 * tangX + m12 * tangY + m13 * tangZ);
				ty += weight * (m21 * tangX + m22 * tangY + m23 * tangZ);
				tz += weight * (m31 * tangX + m32 * tangY + m33 * tangZ);
			}
			j++;
		}

		targetData[i] = vx;
		targetData[i + 1] = vy;
		targetData[i + 2] = vz;
		targetData[i + 3] = nx;
		targetData[i + 4] = ny;
		targetData[i + 5] = nz;
		targetData[i + 6] = tx;
		targetData[i + 7] = ty;
		targetData[i + 8] = tz;
	}
}