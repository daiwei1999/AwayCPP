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

	m_numTotalJoints = m_skeleton->m_joints.size();
	m_globalPose.m_jointPoses.resize(m_numTotalJoints);
	m_globalMatrices.resize(m_numTotalJoints * 12);
	for (int i = 0, j = 0; i < m_numTotalJoints; i++)
	{
		m_globalMatrices[j++] = 1;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 1;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 0;
		m_globalMatrices[j++] = 1;
		m_globalMatrices[j++] = 0;
	}
}

SkeletonAnimator::~SkeletonAnimator()
{
	for (auto state : m_subGeomAnimStates)
		delete state.second;
}

SkeletonPose& SkeletonAnimator::getGlobalPose()
{
	if (m_globalPropertiesDirty)
		updateGlobalProperties();

	return m_globalPose;
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

	SkinnedSubGeometry* skinnedGeom = (SkinnedSubGeometry*)(((SubMesh*)renderable)->getSubGeometry());

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
		context->setProgramConstantsFromVector(ProgramType::VERTEX, vertexConstantOffset, m_globalMatrices.data(), m_skeleton->m_numSkinningJoints * 3);
		skinnedGeom->activateJointIndexBuffer(vertexStreamOffset, context);
		skinnedGeom->activateJointWeightsBuffer(vertexStreamOffset + 1, context);
	}
}

void SkeletonAnimator::testGPUCompatibility(MaterialPassBase* pass)
{
	if (m_forceCPU || m_jointsPerVertex > 4 || pass->getUsedVertexConstantCount() + m_skeleton->m_numSkinningJoints * 3 > 128)
		m_animationSet->cancelGPUCompatibility();
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
	localToGlobalPose(((SkeletonClipState*)m_activeState)->getSkeletonPose(m_skeleton));

	float ox, oy, oz, ow, t, xy2, xz2, xw2, yz2, yw2, zw2;
	float n11, n12, n13, n21, n22, n23, n31, n32, n33;
	float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34;
	std::vector<JointPose>& globalPoses = m_globalPose.m_jointPoses;
	std::vector<SkeletonJoint*>& joints = m_skeleton->m_joints;
	for (int i = 0, j = 0; i < m_numTotalJoints; i++)
	{
		JointPose& pose = globalPoses[i];
		Quaternion& quat = pose.m_orientation;
		Vector3D& vec = pose.m_translation;
		ox = quat.m_x;
		oy = quat.m_y;
		oz = quat.m_z;
		ow = quat.m_w;

		xy2 = (t = 2 * ox) * oy;
		xz2 = t * oz;
		xw2 = t * ow;
		yz2 = (t = 2 * oy) * oz;
		yw2 = t * ow;
		zw2 = 2 * oz * ow;

		ox *= ox;
		oy *= oy;
		oz *= oz;
		ow *= ow;

		n11 = (t = ox - oy) - oz + ow;
		n12 = xy2 - zw2;
		n13 = xz2 + yw2;
		n21 = xy2 + zw2;
		n22 = -t - oz + ow;
		n23 = yz2 - xw2;
		n31 = xz2 - yw2;
		n32 = yz2 + xw2;
		n33 = -ox - oy + oz + ow;

		// prepend inverse bind pose
		float(&raw)[12] = joints[i]->m_inverseBindPose;
		m11 = raw[0];
		m12 = raw[3];
		m13 = raw[6];
		m14 = raw[9];
		m21 = raw[1];
		m22 = raw[4];
		m23 = raw[7];
		m24 = raw[10];
		m31 = raw[2];
		m32 = raw[5];
		m33 = raw[8];
		m34 = raw[11];

		m_globalMatrices[j] = n11 * m11 + n12 * m21 + n13 * m31;
		m_globalMatrices[j + 1] = n11 * m12 + n12 * m22 + n13 * m32;
		m_globalMatrices[j + 2] = n11 * m13 + n12 * m23 + n13 * m33;
		m_globalMatrices[j + 3] = n11 * m14 + n12 * m24 + n13 * m34 + vec.m_x;
		m_globalMatrices[j + 4] = n21 * m11 + n22 * m21 + n23 * m31;
		m_globalMatrices[j + 5] = n21 * m12 + n22 * m22 + n23 * m32;
		m_globalMatrices[j + 6] = n21 * m13 + n22 * m23 + n23 * m33;
		m_globalMatrices[j + 7] = n21 * m14 + n22 * m24 + n23 * m34 + vec.m_y;
		m_globalMatrices[j + 8] = n31 * m11 + n32 * m21 + n33 * m31;
		m_globalMatrices[j + 9] = n31 * m12 + n32 * m22 + n33 * m32;
		m_globalMatrices[j + 10] = n31 * m13 + n32 * m23 + n33 * m33;
		m_globalMatrices[j + 11] = n31 * m14 + n32 * m24 + n33 * m34 + vec.m_z;
		j += 12;
	}
}

void SkeletonAnimator::localToGlobalPose(SkeletonPose& sourcePose)
{
	std::vector<JointPose>& localPoses = sourcePose.m_jointPoses;
	std::vector<JointPose>& globalPoses = m_globalPose.m_jointPoses;
	std::vector<SkeletonJoint*>& joints = m_skeleton->m_joints;
	std::vector<int>& indexTable = m_skeleton->m_indexTable;

	int index, parentIndex;
	float x2, y2, z2, w2, x3, y3, z3, w3, x1, y1, z1, w1;
	for (int i = 0; i < m_numTotalJoints; i++)
	{
		index = indexTable[i];
		JointPose& currentPose = localPoses[index];
		Vector3D& ct = currentPose.m_translation;
		Quaternion& cq = currentPose.m_orientation;

		JointPose& globalJointPose = globalPoses[index];
		Quaternion& q = globalJointPose.m_orientation;
		Vector3D& t = globalJointPose.m_translation;

		parentIndex = joints[index]->m_parentIndex;
		if (parentIndex < 0)
		{
			q.m_x = cq.m_x;
			q.m_y = cq.m_y;
			q.m_z = cq.m_z;
			q.m_w = cq.m_w;
			t.m_x = ct.m_x;
			t.m_y = ct.m_y;
			t.m_z = ct.m_z;
		}
		else
		{
			// append parent pose
			JointPose& parentPose = globalPoses[parentIndex];

			// rotate point
			Quaternion& pq = parentPose.m_orientation;
			x2 = pq.m_x;
			y2 = pq.m_y;
			z2 = pq.m_z;
			w2 = pq.m_w;
			x3 = ct.m_x;
			y3 = ct.m_y;
			z3 = ct.m_z;

			w1 = x2 * x3 + y2 * y3 + z2 * z3;
			x1 = w2 * x3 + y2 * z3 - z2 * y3;
			y1 = w2 * y3 - x2 * z3 + z2 * x3;
			z1 = w2 * z3 + x2 * y3 - y2 * x3;

			// append parent translation
			Vector3D& pt = parentPose.m_translation;
			t.m_x = w1 * x2 + x1 * w2 - y1 * z2 + z1 * y2 + pt.m_x;
			t.m_y = w1 * y2 + x1 * z2 + y1 * w2 - z1 * x2 + pt.m_y;
			t.m_z = w1 * z2 - x1 * y2 + y1 * x2 + z1 * w2 + pt.m_z;

			// append parent orientation
			x3 = cq.m_x;
			y3 = cq.m_y;
			z3 = cq.m_z;
			w3 = cq.m_w;

			q.m_w = w2 * w3 - x2 * x3 - y2 * y3 - z2 * z3;
			q.m_x = w2 * x3 + x2 * w3 + y2 * z3 - z2 * y3;
			q.m_y = w2 * y3 - x2 * z3 + y2 * w3 + z2 * x3;
			q.m_z = w2 * z3 + x2 * y3 - y2 * x3 + z2 * w3;
		}
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
				m11 = m_globalMatrices[mtxOffset];
				m12 = m_globalMatrices[mtxOffset + 1];
				m13 = m_globalMatrices[mtxOffset + 2];
				m21 = m_globalMatrices[mtxOffset + 4];
				m22 = m_globalMatrices[mtxOffset + 5];
				m23 = m_globalMatrices[mtxOffset + 6];
				m31 = m_globalMatrices[mtxOffset + 8];
				m32 = m_globalMatrices[mtxOffset + 9];
				m33 = m_globalMatrices[mtxOffset + 10];
				vx += weight * (m11 * vertX + m12 * vertY + m13 * vertZ + m_globalMatrices[mtxOffset + 3]);
				vy += weight * (m21 * vertX + m22 * vertY + m23 * vertZ + m_globalMatrices[mtxOffset + 7]);
				vz += weight * (m31 * vertX + m32 * vertY + m33 * vertZ + m_globalMatrices[mtxOffset + 11]);
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