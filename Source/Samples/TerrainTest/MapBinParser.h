#pragma once

#include "Common.h"
#include "Vector3D.h"

struct Chunk
{
	unsigned short m_id;
	unsigned int m_size;
};

struct SurfaceData
{
	std::string m_diffuseMap;
	float m_textureTiling;
	float m_distMaskStrength;
};

struct LightMapData
{
	float m_scaleR;
	float m_scaleG;
	float m_scaleB;
};

struct TransformData
{
	away::Vector3D m_position;
	away::Vector3D m_rotation;
	away::Vector3D m_scale;
};

struct BeastData
{
	unsigned char m_frameIndex;
	float m_scaleU;
	float m_scaleV;
	float m_offsetU;
	float m_offsetV;
};

struct ShakyData
{
	float m_amplitudeX;
	float m_amplitudeY;
	float m_amplitudeZ;
	float m_frequency;
	float m_phaseFactor;
};

struct BuildingData
{
	std::string m_name;
	TransformData m_transform;
	std::vector<BeastData> m_beast;
	bool m_bShaky = false;
	ShakyData m_shakyData;
};

struct InstanceData
{
	std::string m_name;
	std::vector<TransformData> m_instanceBuildings;
	away::Vector3D m_shakyData;
};

struct EffectData
{
	std::string m_name;
	TransformData m_transform;
};

struct BlockData
{
	std::vector<unsigned char> m_surfaceIndents;
	BeastData m_terrainBeast;
	std::vector<BuildingData> m_buildings;
	std::vector<EffectData> m_effects;
	std::vector<InstanceData> m_instances;
};

class MapBinParser
{
public:
	MapBinParser();

	void parse(away::ByteArray* bytes);
	int getSectorGrid() { return m_sectorGrid; }
	int getBlockGrid() { return m_blockGrid; }
	float getVertexSpace() { return m_vertexSpace; }
	float getMinHeight() { return m_minHeight; }
	float getMaxHeight() { return m_maxHeight; }
	int getWalkGridSize() { return m_walkGridSize; }

	std::vector<SurfaceData>& getSurfaceData() { return m_surfaces; }
	std::vector<LightMapData>& getTerrainLightMapData() { return m_terrainLightMaps; }
	std::vector<LightMapData>& getBuildingLightMapData() { return m_buildingLightMaps; }
	std::vector<BlockData>& getBlockData() { return m_blocks; }

private:
	void parseChunk(Chunk& chunk);
	void parseTransformData(TransformData& transform, bool uniformScale);

	void parseConfig();
	void parseWalkGrid();
	void parseSurfaces();
	void parseLightMaps(std::vector<LightMapData>& lightmaps);
	void parseBlocks(unsigned int end);
	void parseBlock(unsigned int end);
	void parseSurfaceIndents(std::vector<unsigned char>& surfaceIndents);
	void parseBeastData(BeastData& beastData);
	void parseBuildings(std::vector<BuildingData>& buildings, unsigned int end);
	void parseBuilding(std::vector<BuildingData>& buildings, unsigned int end);
	void parseBuildingBasic(BuildingData& building);
	void parseBuildingBeast(std::vector<BeastData>& beastDatas);
	void parseShakyData(ShakyData& shakyData);
	void parseEffects(std::vector<EffectData>& effects, unsigned int end);
	void parseEffect(std::vector<EffectData>& effects, unsigned int end);
	void parseEffectBasic(EffectData& effect);
	void parseInstances(std::vector<InstanceData>& instances, unsigned int end);
	void parseInstance(std::vector<InstanceData>& instances, unsigned int end);

private:
	away::ByteArray* m_body;
	int m_sectorGrid;
	int m_blockGrid;
	float m_vertexSpace;
	float m_minHeight;
	float m_maxHeight;
	int m_walkGridSize;
	std::vector<SurfaceData> m_surfaces;
	std::vector<LightMapData> m_terrainLightMaps;
	std::vector<LightMapData> m_buildingLightMaps;
	std::vector<BlockData> m_blocks;
};