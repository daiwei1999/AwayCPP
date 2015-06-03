#include "TerrainMesh.h"
#include "ByteArray.h"
#include "Geometry.h"
#include "ATFTexture.h"
#include "TextureMaterial.h"
#include "TerrainDiffuseMethod.h"
#include "CompactSubGeometry.h"
#include "BoundingVolumeBase.h"
#include "MapBinParser.h"

TerrainMesh::TerrainMesh(int blockGrid, float vertexSpace) : away::Mesh(new away::Geometry())
{
	m_blockGrid = blockGrid;
	m_vertexSpace = vertexSpace;
}

float TerrainMesh::getTerrainHeight(float x, float z)
{
	away::BoundingVolumeBase* bounds = getBounds();
	away::Vector3D* min = bounds->getMin();
	away::Vector3D* max = bounds->getMax();
	if (x < min->m_x || x > max->m_x || z < min->m_z || z > max->m_z)
		return 0;

	x = (x - min->m_x) / m_vertexSpace;
	z = (z - min->m_z) / m_vertexSpace;
	int col = (int)x;
	int row = (int)z;
	float wx = x - col;
	float wz = z - row;

	int numVertexPerRow = m_blockGrid + 1;
	away::ISubGeometry* subGeom = m_geometry->getSubGeometries()[0];
	int stride = subGeom->getVertexStride();
	float* vertices = subGeom->getVertexData();
	int idx = (row * numVertexPerRow + col) * stride + 1;
	float lb = vertices[idx];
	float rb = vertices[idx + stride];
	idx += numVertexPerRow * stride;
	float lt = vertices[idx];
	float rt = vertices[idx + stride];
	return (lb * (1 - wx) + rb * wx) * (1 - wz) + (lt * (1 - wx) + rt * wx) * wz;
}

void TerrainMesh::buildGeometry(away::ByteArray& heightBytes, int sectorGrid, float minHeight, float maxHeight, unsigned short* indices, int blockId)
{
	int numBlockPerRow = sectorGrid / m_blockGrid;
	int i = 0, j = 0, row = blockId / numBlockPerRow, col = blockId % numBlockPerRow;
	float hScale = (maxHeight - minHeight) * m_vertexSpace / 65535;
	float hOffset = minHeight * m_vertexSpace;
	float offsetX = (col * m_blockGrid - sectorGrid / 2) * m_vertexSpace;
	float offsetZ = (row * m_blockGrid - sectorGrid / 2) * m_vertexSpace;
	int offset = (sectorGrid - m_blockGrid) * 2;
	int numVertexPerRow = m_blockGrid + 1;
	int numVertices = numVertexPerRow * numVertexPerRow;
	float* vertices = new float[numVertices * 3];
	float* uvs = new float[numVertices * 2];

	heightBytes.setPosition(((sectorGrid + 1) * row + col) * m_blockGrid * 2);
	for (row = 0; row < numVertexPerRow; row++)
	{
		for (col = 0; col < numVertexPerRow; col++)
		{
			vertices[i++] = offsetX + col * m_vertexSpace;
			vertices[i++] = heightBytes.readUnsignedShort() * hScale + hOffset;
			vertices[i++] = offsetZ + row * m_vertexSpace;
			uvs[j++] = (float)col / m_blockGrid;
			uvs[j++] = 1 - (float)row / m_blockGrid;
		}
		heightBytes.setPosition(heightBytes.getPosition() + offset);
	}

	away::CompactSubGeometry* subGeom = new away::CompactSubGeometry();
	subGeom->updateIndexData(indices, m_blockGrid * m_blockGrid * 6);
	subGeom->fromVectors(vertices, nullptr, nullptr, uvs, nullptr, numVertices);
	m_geometry->addSubGeometry(subGeom);
}

void TerrainMesh::buildMaterial(away::ATFTexture* blendMap, std::vector<away::ATFTexture*>& layerMaps, std::vector<float>& layerTilings, std::vector<float>& distMaskStrengths,
	away::ATFTexture* lightMap, BeastData& beastData, LightMapData& lightMapData, int numBlockPerRow, int blockId)
{
	int numLayers = layerMaps.size();
	away::TerrainDiffuseMethod* method = new away::TerrainDiffuseMethod(numLayers, true, false);
	method->setBlendMapTexture(blendMap);
	method->setLightMapTexture(lightMap);
	method->setLightUVData(beastData.m_scaleU, beastData.m_scaleV, beastData.m_offsetU, beastData.m_offsetV);
	method->setLightScaleData(lightMapData.m_scaleR, lightMapData.m_scaleG, lightMapData.m_scaleB);

	int row = blockId / numBlockPerRow;
	int col = blockId % numBlockPerRow;
	int blendMaskSize = blendMap->getWidth();
	float layerOffsetU = (float)col / numBlockPerRow;
	float layerOffsetV = (float)(numBlockPerRow - row - 1) / numBlockPerRow;
	float layerScaleUV = 1.f / numBlockPerRow;
	float bias = 1.f / blendMaskSize;
	float blendScaleUV = layerScaleUV - bias * 2;
	method->setBlendUVData(blendScaleUV, blendScaleUV, layerOffsetU + bias, layerOffsetV + bias);
	method->setLayerUVData(layerScaleUV, layerScaleUV, layerOffsetU, layerOffsetV);

	for (int i = 0; i < numLayers; i++)
	{
		method->setLayerMapTexture(i, layerMaps[i]);
		method->setLayerTiling(i, layerTilings[i]);
		method->setDistMaskStrength(i, distMaskStrengths[i]);
	}

	away::TextureMaterial* material = new away::TextureMaterial();
	material->setDiffuseMethod(method);
	setMaterial(material);
}