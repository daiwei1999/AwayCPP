#include "SegmentSet.h"
#include "Segment.h"
#include "IContext.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderableNode.h"
#include "BoundingSphere.h"
#include "SegmentMaterial.h"

USING_AWAY_NAMESPACE

SegmentSet::SegmentSet()
{
	m_bounds = new BoundingSphere();
	m_worldBounds = new BoundingSphere();
	m_vertexContext = nullptr;
	m_vertexBuffer = nullptr;
	m_material = new SegmentMaterial();
}

void SegmentSet::addSegment(Segment* segment)
{
	segment->m_index = m_vertices.size();
	segment->m_segmentSet = this;
	m_segments.push_back(segment);

	m_vertices.resize(m_segments.size() * 12);
	updateSegment(segment);
}

void SegmentSet::removeSegment(Segment* segment)
{
	// update index of segments behind the removed segment
	int index = segment->m_index / 12;
	m_segments.erase(m_segments.begin() + index);
	for (size_t i = index; i < m_segments.size(); i++)
		m_segments[i]->m_index -= 12;
	
	auto first = m_vertices.begin() + segment->m_index;
	m_vertices.erase(first, first + 12);

	m_vertexBufferDirty = true;
}

void SegmentSet::removeAllSegments()
{
	m_segments.clear();
	m_vertices.clear();
}

void SegmentSet::updateSegment(Segment* segment)
{
	Vector3D& start = segment->m_start;
	Vector3D& end = segment->m_end;
	unsigned int index = segment->m_index;

	m_vertices[index++] = start.m_x;
	m_vertices[index++] = start.m_y;
	m_vertices[index++] = start.m_z;
	m_vertices[index++] = segment->m_startR;
	m_vertices[index++] = segment->m_startG;
	m_vertices[index++] = segment->m_startB;

	m_vertices[index++] = end.m_x;
	m_vertices[index++] = end.m_y;
	m_vertices[index++] = end.m_z;
	m_vertices[index++] = segment->m_endR;
	m_vertices[index++] = segment->m_endG;
	m_vertices[index++] = segment->m_endB;
	
	m_vertexBufferDirty = true;
}

void SegmentSet::activateVertexBuffer(int index, IContext* context)
{
	if (m_vertexContext != context || m_vertexBufferDirty)
	{
		int numVertices = m_segments.size() * 2;
		m_vertexBuffer = context->createVertexBuffer(numVertices, 24);
		m_vertexBuffer->uploadFromVector(m_vertices.data(), 0, numVertices);
		m_vertexContext = context;
		m_vertexBufferDirty = false;
	}
	context->setVertexBufferAt(0, m_vertexBuffer, 0, VertexBufferFormat::FLOAT_3);
	context->setVertexBufferAt(1, m_vertexBuffer, 12, VertexBufferFormat::FLOAT_3);
}

EntityNode* SegmentSet::createEntityPartitionNode()
{
	return new RenderableNode(this);
}

void SegmentSet::updateBounds()
{
	float minX, minY, minZ, maxX, maxY, maxZ;
	minX = minY = minZ = MathConsts::Infinity;
	maxX = maxY = maxZ = -MathConsts::Infinity;
	int len = m_vertices.size();
	for (int i = 0; i < len; i += 4)
	{
		float v = m_vertices[i++];
		if (v < minX)
			minX = v;
		else if (v > maxX)
			maxX = v;

		v = m_vertices[i++];
		if (v < minY)
			minY = v;
		else if (v > maxY)
			maxY = v;

		v = m_vertices[i];
		if (v < minZ)
			minZ = v;
		else if (v > maxZ)
			maxZ = v;
	}

	if (minX != MathConsts::Infinity)
		m_bounds->fromExtremes(minX, minY, minZ, maxX, maxY, maxZ);
	else
		m_bounds->fromExtremes(-.5f, -.5f, -.5f, .5f, .5f, .5f);

	m_boundsInvalid = false;
}