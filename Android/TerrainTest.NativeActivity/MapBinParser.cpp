#include "MapBinParser.h"
#include "ByteArray.h"

MapBinParser::MapBinParser()
{

}

void MapBinParser::parse(away::ByteArray* bytes)
{
	unsigned int size = bytes->getLength();
	bytes->setPosition(3);
	unsigned char version = bytes->readUnsignedByte();
	unsigned char compression = bytes->readUnsignedByte();
	bytes->setPosition(bytes->getPosition() + 3);
	if (bytes->readUnsignedInt() != size - 12)
		assert(false); // MAP body length does not match header integrity field

	switch (compression)
	{
	case 0:
		m_body = bytes;
		break;
	default:
		assert(false); // Unsupported compression format
	}

	while (m_body->getPosition() < size)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0x5000:
			parseConfig();
			break;
		case 0x6000:
			parseWalkGrid();
			break;
		case 0x7000:
			parseSurfaces();
			break;
		case 0x8000:
			parseLightMaps(m_terrainLightMaps);
			break;
		case 0x9000:
			parseLightMaps(m_buildingLightMaps);
			break;
		case 0xA000:
			parseBlocks(m_body->getPosition() + chunk.m_size);
			break;
		default: // skip unknown chunk
			m_body->setPosition(m_body->getPosition() + chunk.m_size);
		}
	}
}

void MapBinParser::parseChunk(Chunk& chunk)
{
	chunk.m_id = m_body->readUnsignedShort();
	chunk.m_size = m_body->readUnsignedInt();
}

void MapBinParser::parseTransformData(TransformData& transform, bool uniformScale)
{
	transform.m_position.m_x = m_body->readFloat();
	transform.m_position.m_y = m_body->readFloat();
	transform.m_position.m_z = m_body->readFloat();
	transform.m_rotation.m_x = m_body->readFloat();
	transform.m_rotation.m_y = m_body->readFloat();
	transform.m_rotation.m_z = m_body->readFloat();
	transform.m_rotation.m_w = m_body->readFloat();

	if (uniformScale)
	{
		transform.m_scale.m_x = m_body->readFloat();
		transform.m_scale.m_y = transform.m_scale.m_x;
		transform.m_scale.m_z = transform.m_scale.m_x;
	}
	else
	{
		transform.m_scale.m_x = m_body->readFloat();
		transform.m_scale.m_y = m_body->readFloat();
		transform.m_scale.m_z = m_body->readFloat();
	}
}

void MapBinParser::parseConfig()
{
	m_sectorGrid = m_body->readUnsignedInt();
	m_blockGrid = m_body->readUnsignedInt();
	m_vertexSpace = m_body->readFloat();
	m_minHeight = m_body->readFloat();
	m_maxHeight = m_body->readFloat();
}

void MapBinParser::parseWalkGrid()
{
	m_walkGridSize = m_body->readUnsignedInt();
	m_body->setPosition(m_body->getPosition() + 4);
}

void MapBinParser::parseSurfaces()
{
	unsigned char numSurfaces = m_body->readUnsignedByte();
	for (unsigned char i = 0; i < numSurfaces; i++)
	{
		char name[256] = { 0 };
		m_body->readString(name);
		float textureTiling = m_body->readFloat();
		float distMaskStrength = m_body->readFloat();
		m_surfaces.push_back({ name, textureTiling, distMaskStrength });
	}
}

void MapBinParser::parseLightMaps(std::vector<LightMapData>& lightmaps)
{
	unsigned char numItems = m_body->readUnsignedByte();
	for (unsigned char i = 0; i < numItems; i++)
	{
		float scaleR = m_body->readFloat();
		float scaleG = m_body->readFloat();
		float scaleB = m_body->readFloat();
		lightmaps.push_back({ scaleR, scaleG, scaleB });
	}
}

void MapBinParser::parseBlocks(unsigned int end)
{
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA100:
			parseBlock(m_body->getPosition() + chunk.m_size);
			break;
		default:
			assert(false);
		}
	}
}

void MapBinParser::parseBlock(unsigned int end)
{
	BlockData block;
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA200:
			parseSurfaceIndents(block.m_surfaceIndents);
			break;
		case 0xA300:
			parseBeastData(block.m_terrainBeast);
			break;
		case 0xA400:
			parseBuildings(block.m_buildings, m_body->getPosition() + chunk.m_size);
			break;
		case 0xA500:
			parseEffects(block.m_effects, m_body->getPosition() + chunk.m_size);
			break;
		case 0xA600:
			parseInstances(block.m_instances, m_body->getPosition() + chunk.m_size);
			break;
		default: // skip unknown chunk
			m_body->setPosition(m_body->getPosition() + chunk.m_size);
		}
	}
	m_blocks.push_back(block);
}

void MapBinParser::parseSurfaceIndents(std::vector<unsigned char>& surfaceIndents)
{
	unsigned char numItems = m_body->readUnsignedByte();
	for (unsigned char i = 0; i < numItems; i++)
		surfaceIndents.push_back(m_body->readUnsignedByte());
}

void MapBinParser::parseBeastData(BeastData& beastData)
{
	beastData.m_frameIndex = m_body->readUnsignedByte();
	beastData.m_offsetU = m_body->readFloat();
	beastData.m_offsetV = m_body->readFloat();
	beastData.m_scaleU = m_body->readFloat();
	beastData.m_scaleV = m_body->readFloat();
}

void MapBinParser::parseBuildings(std::vector<BuildingData>& buildings, unsigned int end)
{
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA410:
			parseBuilding(buildings, m_body->getPosition() + chunk.m_size);
			break;
		default:
			assert(false);
		}
	}
}

void MapBinParser::parseBuilding(std::vector<BuildingData>& buildings, unsigned int end)
{
	BuildingData building;
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA411:
			parseBuildingBasic(building);
			break;
		case 0xA412:
			parseBuildingBeast(building.m_beast);
			break;
		case 0xA413:
			building.m_bShaky = true;
			parseShakyData(building.m_shakyData);
			break;
		default: // skip unknown chunk
			m_body->setPosition(m_body->getPosition() + chunk.m_size);
		}
	}
	buildings.push_back(building);
}

void MapBinParser::parseBuildingBasic(BuildingData& building)
{
	char name[256] = { 0 };
	m_body->readString(name);
	building.m_name = name;
	parseTransformData(building.m_transform, true);
}

void MapBinParser::parseBuildingBeast(std::vector<BeastData>& beastDatas)
{
	unsigned char numItems = m_body->readUnsignedByte();
	for (unsigned char i = 0; i < numItems; i++)
	{
		BeastData beastData;
		parseBeastData(beastData);
		beastDatas.push_back(beastData);
	}
}

void MapBinParser::parseShakyData(ShakyData& shakyData)
{
	shakyData.m_amplitudeX = m_body->readFloat();
	shakyData.m_amplitudeY = m_body->readFloat();
	shakyData.m_amplitudeZ = m_body->readFloat();
	shakyData.m_frequency = m_body->readFloat();
	shakyData.m_phaseFactor = m_body->readFloat();
}

void MapBinParser::parseEffects(std::vector<EffectData>& effects, unsigned int end)
{
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA510:
			parseEffect(effects, m_body->getPosition() + chunk.m_size);
			break;
		default:
			assert(false);
		}
	}
}

void MapBinParser::parseEffect(std::vector<EffectData>& effects, unsigned int end)
{
	EffectData effect;
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA511:
			parseEffectBasic(effect);
			break;
		default: // skip unknown chunk
			m_body->setPosition(m_body->getPosition() + chunk.m_size);
		}
	}
	effects.push_back(effect);
}

void MapBinParser::parseEffectBasic(EffectData& effect)
{
	char name[256] = { 0 };
	m_body->readString(name);
	effect.m_name = name;
	parseTransformData(effect.m_transform, false);
}

void MapBinParser::parseInstances(std::vector<InstanceData>& instances, unsigned int end)
{
	while (m_body->getPosition() < end)
	{
		Chunk chunk;
		parseChunk(chunk);
		switch (chunk.m_id)
		{
		case 0xA610:
			parseInstance(instances, m_body->getPosition() + chunk.m_size);
			break;
		default:
			assert(false);
		}
	}
}

void MapBinParser::parseInstance(std::vector<InstanceData>& instances, unsigned int end)
{

}