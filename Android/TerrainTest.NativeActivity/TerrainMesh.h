#pragma once

#include "Common.h"
#include "Mesh.h"

struct BeastData;
struct LightMapData;

class TerrainMesh : public away::Mesh
{
public:
	TerrainMesh(int blockGrid, float vertexSpace);

	float getTerrainHeight(float x, float z);
	void buildGeometry(away::ByteArray& heightBytes, int sectorGrid, float minHeight, float maxHeight, unsigned short* indices, int blockId);
	void buildMaterial(away::ATFTexture* blendMap, std::vector<away::ATFTexture*>& layerMaps, std::vector<float>& layerTilings, std::vector<float>& distMaskStrengths,
		away::ATFTexture* lightMap, BeastData& beastData, LightMapData& lightMapData, int numBlockPerRow, int blockId);

private:
	int m_blockGrid;
	float m_vertexSpace;
};