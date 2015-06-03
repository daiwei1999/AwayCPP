#include "KLDParser.h"
#include "ByteArray.h"
#include "CompactSubGeometry.h"
#include "Mesh.h"
#include "Geometry.h"
#include "AxisAlignedBoundingBox.h"

KLDParser::KLDParser(away::Mesh* mesh)
{
	m_mesh = mesh;
	m_indices = nullptr;
	m_vertices = nullptr;
	m_normals = nullptr;
	m_uvs = nullptr;
	m_secondaryUVs = nullptr;
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
		default:
			m_body->setPosition(m_body->getPosition() + size);
			break;
		}
	}

	away::CompactSubGeometry* subGeom = new away::CompactSubGeometry();
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