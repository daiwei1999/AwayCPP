#include "SkyBox.h"
#include "NullBounds.h"
#include "SubGeometry.h"
#include "SkyBoxMaterial.h"
#include "SkyBoxNode.h"

USING_AWAY_NAMESPACE

SkyBox::SkyBox(CubeTextureBase* cubeMap) : m_vertices({ -1, 1, -1, 1, 1, -1, 1, 1, 1, -1, 1, 1, -1, -1, -1, 1, -1, -1, 1, -1, 1, -1, -1, 1 }),
										   m_indices({ 0, 1, 2, 2, 3, 0, 6, 5, 4, 4, 7, 6, 2, 6, 7, 7, 3, 2, 4, 5, 1, 1, 0, 4, 4, 0, 3, 3, 7, 4, 2, 1, 5, 5, 6, 2 })
{
	m_bounds = new NullBounds();
	m_worldBounds = new NullBounds();

	m_geometry = new SubGeometry();
	m_geometry->updateVertexData(m_vertices.data(), 8);
	m_geometry->updateIndexData(m_indices.data(), 36);

	m_material = new SkyBoxMaterial(cubeMap);
}

MaterialBase* SkyBox::getMaterial()
{
	return m_material;
}

int SkyBox::getTriangleCount()
{
	return m_geometry->getTriangleCount();
}

void SkyBox::activateVertexBuffer(int index, IContext* context)
{
	m_geometry->activateVertexBuffer(index, context);
}

IndexBuffer* SkyBox::getIndexBuffer(IContext* context)
{
	return m_geometry->getIndexBuffer(context);
}

float* SkyBox::getVertexData()
{
	return m_geometry->getVertexData();
}

float* SkyBox::getVertexNormalData()
{
	return m_geometry->getVertexNormalData();
}

float* SkyBox::getVertexTangentData()
{
	return m_geometry->getVertexTangentData();
}

unsigned short* SkyBox::getIndexData()
{
	return m_geometry->getIndexData();
}

float* SkyBox::getUVData()
{
	return m_geometry->getUVData();
}

EntityNode* SkyBox::createEntityPartitionNode()
{
	return new SkyBoxNode(this);
}

void SkyBox::updateBounds()
{
	m_boundsInvalid = false;
}