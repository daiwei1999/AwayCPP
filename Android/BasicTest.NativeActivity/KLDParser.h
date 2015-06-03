#pragma once

#include "Common.h"

struct Chunk
{
	unsigned short m_id;
	unsigned int m_size;
};

class KLDParser
{
public:
	KLDParser(away::Mesh* mesh);

	void parse(away::ByteArray* bytes);

private:
	void parseChunk(Chunk& chunk);
	void parseSubMesh(unsigned int end);
	void parseBounds();
	void parseSkeleton(unsigned int end);
	void parseAnimation(unsigned int end);
	void parseJointTags(unsigned int end);
	void parseIndices(unsigned int size);
	void parseVertices(unsigned int size);
	void parseUVs(unsigned int size);
	void parseSecondaryUVs(unsigned int size);
	void parseNormals(unsigned int size);
	void parseSkin();

private:
	away::Mesh* m_mesh;
	away::ByteArray* m_body;
	int m_numIndices;
	unsigned short* m_indices = nullptr;
	int m_numVertices;
	int m_numJoints = 0;
	float* m_vertices = nullptr;
	float* m_normals = nullptr;
	float* m_uvs = nullptr;
	float* m_secondaryUVs = nullptr;
	float* m_jointWeights = nullptr;
	unsigned char* m_jointIndices = nullptr;
	unsigned char m_jointsPerVertex;
};