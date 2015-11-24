#include "KLDParser.h"
#include "ByteArray.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Skeleton.h"
#include "SkinnedSubGeometry.h"
#include "SkeletonAnimator.h"
#include "SkeletonClipNode.h"
#include "SkeletonAnimationSet.h"
#include "AxisAlignedBoundingBox.h"
#include "JointTag.h"

KLDParser::KLDParser(away::Mesh* mesh)
{
	m_mesh = mesh;
}

void KLDParser::parse(away::ByteArray* bytes)
{
	unsigned int size = bytes->getLength();
	bytes->setPosition(3);
	unsigned char version = bytes->readUnsignedByte();
	if (version < 2)
		assert(false); // KLDParser cannot parse file with version < 2

	unsigned char compression = bytes->readUnsignedByte();
	bytes->setPosition(bytes->getPosition() + 3);
	if (bytes->readUnsignedInt() != size - 12)
		assert(false); // KLD body length does not match header integrity field

	switch (compression)
	{
	case 0:
		m_body = bytes;
		break;
	default:
		assert(false); // Unsupported compression format
		break;
	}

	Chunk chunk;
	unsigned int end;
	while (m_body->getPosition() < size)
	{
		parseChunk(chunk);
		end = m_body->getPosition() + chunk.m_size;
		switch (chunk.m_id)
		{
		case 0x1000:
			parseSubMesh(end);
			break;
		case 0x2000:
			parseBounds();
			break;
		case 0x3000:
			parseSkeleton(end);
			break;
		case 0x4000:
			parseAnimation(end);
			break;
		case 0x5000:
			parseJointTags(end);
			break;
		default:
			m_body->setPosition(m_body->getPosition() + chunk.m_size);
			break;
		}
	}
}

void KLDParser::parseChunk(Chunk& chunk)
{
	chunk.m_id = m_body->readUnsignedShort();
	chunk.m_size = m_body->readUnsignedInt();
}

void KLDParser::parseSubMesh(unsigned int end)
{
	Chunk chunk;
	unsigned int size;
	while (m_body->getPosition() < end)
	{
		parseChunk(chunk);
		size = chunk.m_size;
		switch (chunk.m_id)
		{
		case 0x1200:
			parseIndices(size);
			break;
		case 0x1300:
			parseVertices(size);
			break;
		case 0x1400:
			parseUVs(size);
			break;
		case 0x1500:
			parseSecondaryUVs(size);
			break;
		case 0x1700:
			parseNormals(size);
			break;
		case 0x1800:
			parseSkin();
			break;
		default:
			m_body->setPosition(m_body->getPosition() + size);
			break;
		}
	}

	away::CompactSubGeometry* subGeom;
	if (m_jointIndices && m_jointWeights)
	{
		subGeom = new away::SkinnedSubGeometry(m_jointsPerVertex);
		static_cast<away::SkinnedSubGeometry*>(subGeom)->updateJointIndexData(m_jointIndices);
		static_cast<away::SkinnedSubGeometry*>(subGeom)->updateJointWeightsData(m_jointWeights);
	}
	else
		subGeom = new away::CompactSubGeometry();
	
	subGeom->updateIndexData(m_indices, m_numIndices);
	subGeom->fromVectors(m_vertices, m_normals, nullptr, m_uvs, m_secondaryUVs, m_numVertices);
	m_mesh->getGeometry()->addSubGeometry(subGeom);
}

void KLDParser::parseBounds()
{
	float minX = m_body->readFloat();
	float minY = m_body->readFloat();
	float minZ = m_body->readFloat();
	float maxX = m_body->readFloat();
	float maxY = m_body->readFloat();
	float maxZ = m_body->readFloat();

	away::AxisAlignedBoundingBox bounds;
	bounds.fromExtremes(minX, minY, minZ, maxX, maxY, maxZ);
	m_mesh->setBounds(&bounds);
}

void KLDParser::parseSkeleton(unsigned int end)
{
	away::SkeletonJoint* joint;
	away::Skeleton* skeleton = new away::Skeleton();
	skeleton->m_numSkinningJoints = m_body->readUnsignedByte();
	while (m_body->getPosition() < end)
	{
		char name[256] = { 0 };
		m_body->readString(name);

		joint = new away::SkeletonJoint();
		joint->m_name = name;
		joint->m_parentIndex = m_body->readUnsignedByte() - 1;
		m_body->readBlock(joint->m_inverseBindPose, 48);
		skeleton->m_joints.push_back(joint);
		m_numJoints++;
	}
	skeleton->buildIndexTable();
	m_mesh->setAnimator(new away::SkeletonAnimator(new away::SkeletonAnimationSet(m_jointsPerVertex), skeleton));
}

void KLDParser::parseAnimation(unsigned int end)
{
	char name[256] = { 0 };
	m_body->readString(name);

	away::SkeletonClipNode* node = new away::SkeletonClipNode();
	node->setName(name);
	node->setLooping(m_body->readBoolean());

	unsigned char frameRate = m_body->readUnsignedByte();
	float duration = 1.f / frameRate;
	while (m_body->getPosition() < end)
	{
		away::SkeletonPose* skeletonPose = new away::SkeletonPose();
		skeletonPose->m_jointPoses.resize(m_numJoints);
		for (int i = 0; i < m_numJoints; i++)
		{
			away::JointPose& jointPose = skeletonPose->m_jointPoses[i];
			m_body->readBlock(&jointPose.m_translation, 12);
			m_body->readBlock(&jointPose.m_orientation, 16);
		}
		node->addFrame(skeletonPose, duration);
	}

	static_cast<away::SkeletonAnimationSet*>(m_mesh->getAnimator()->getAnimationSet())->addAnimation(node);
}

void KLDParser::parseJointTags(unsigned int end)
{
	while (m_body->getPosition() < end)
	{
		char name[256] = { 0 };
		m_body->readString(name);
		int jointIndex = m_body->readUnsignedByte() - 1;
		away::Matrix3D matrix;
		m_body->readBlock(matrix.m_rawData, 12);
		m_body->readBlock(matrix.m_rawData + 4, 12);
		m_body->readBlock(matrix.m_rawData + 8, 12);
		m_body->readBlock(matrix.m_rawData + 12, 12);

		away::JointTag* jointTag = new away::JointTag(jointIndex);
		jointTag->setName(name);
		jointTag->setTransform(matrix);
		m_mesh->addJointTag(jointTag);
	}
}

void KLDParser::parseIndices(unsigned int size)
{
	m_numIndices = size / 2;
	m_indices = new unsigned short[m_numIndices];
	m_body->readBlock(m_indices, size);
}

void KLDParser::parseVertices(unsigned int size)
{
	m_numVertices = size / 12;
	m_vertices = new float[m_numVertices * 3];
	m_body->readBlock(m_vertices, size);
}

void KLDParser::parseUVs(unsigned int size)
{
	m_uvs = new float[size / 4];
	m_body->readBlock(m_uvs, size);
}

void KLDParser::parseSecondaryUVs(unsigned int size)
{
	m_secondaryUVs = new float[size / 4];
	m_body->readBlock(m_secondaryUVs, size);
}

void KLDParser::parseNormals(unsigned int size)
{
	m_normals = new float[size / 4];
	m_body->readBlock(m_normals, size);
}

void KLDParser::parseSkin()
{
	int offset, index, numJoints;
	m_jointsPerVertex = m_body->readUnsignedByte();
	int numItems = m_numVertices * m_jointsPerVertex;
	m_jointIndices = new unsigned char[numItems];
	std::memset(m_jointIndices, 0, numItems * sizeof(unsigned char));
	m_jointWeights = new float[numItems];
	std::memset(m_jointWeights, 0, numItems * sizeof(float));

	for (int i = 0; i < m_numVertices; i++)
	{
		offset = i * m_jointsPerVertex;
		numJoints = m_body->readUnsignedByte();
		for (int j = 0; j < numJoints; j++)
		{
			index = offset + j;
			m_jointIndices[index] = m_body->readUnsignedByte() * 3;
			m_jointWeights[index] = m_body->readFloat();
		}
	}
}