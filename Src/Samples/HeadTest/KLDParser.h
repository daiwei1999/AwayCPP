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
	void parseIndices(unsigned int size);
	void parseVertices(unsigned int size);
	void parseUVs(unsigned int size);
	void parseSecondaryUVs(unsigned int size);
	void parseNormals(unsigned int size);

private:
	away::Mesh* m_mesh;
	away::ByteArray* m_body;
	int m_numIndices;
	unsigned short* m_indices;
	int m_numVertices;
	float* m_vertices;
	float* m_normals;
	float* m_uvs;
	float* m_secondaryUVs;
};