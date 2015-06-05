#include "PlaneGeometry.h"
#include "CompactSubGeometry.h"

USING_AWAY_NAMESPACE

PlaneGeometry::PlaneGeometry(float width, float height, unsigned short segmentsW, unsigned short segmentsH, bool yUp, bool doubleSided)
{
	m_width = width;
	m_height = height;
	m_segmentsW = segmentsW;
	m_segmentsH = segmentsH;
	m_yUp = yUp;
	m_doubleSided = doubleSided;
}

void PlaneGeometry::setWidth(float value)
{
	if (value != m_width)
	{
		m_width = value;
		invalidateGeometry();
	}
}

void PlaneGeometry::setHeight(float value)
{
	if (value != m_height)
	{
		m_height = value;
		invalidateGeometry();
	}
}

void PlaneGeometry::setSegmentsW(unsigned short value)
{
	if (value != m_segmentsW)
	{
		m_segmentsW = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void PlaneGeometry::setSegmentsH(unsigned short value)
{
	if (value != m_segmentsH)
	{
		m_segmentsH = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void PlaneGeometry::setYUp(bool value)
{
	if (value != m_yUp)
	{
		m_yUp = value;
		invalidateGeometry();
	}
}

void PlaneGeometry::setDoubleSided(bool value)
{
	if (value != m_doubleSided)
	{
		m_doubleSided = value;
		invalidateGeometry();
	}
}

void PlaneGeometry::buildGeometry(CompactSubGeometry* target)
{
	unsigned short tw = m_segmentsW + 1;
	int numVertices = (m_segmentsH + 1) * tw;
	int numIndices = m_segmentsH * m_segmentsW * 6;
	int stride = target->getVertexStride();
	int skip = stride - 9;
	if (m_doubleSided)
	{
		numVertices *= 2;
		numIndices *= 2;
	}

	float* vertices;
	unsigned short* indices;
	if (numVertices == target->getVertexCount())
	{
		vertices = target->getVertexData();
		indices = target->getIndexData();
	}
	else
	{
		vertices = new float[numVertices * stride];
		indices = new unsigned short[numIndices];
		invalidateUVs();
	}

	float x, y;
	numIndices = 0;
	int index = target->getVertexOffset();
	unsigned short i, j, base, mult = m_doubleSided ? 2 : 1;
	for (i = 0; i <= m_segmentsH; i++)
	{
		for (j = 0; j <= m_segmentsW; j++)
		{
			x = ((float)j / m_segmentsW - .5f) * m_width;
			y = ((float)i / m_segmentsH - .5f) * m_height;

			vertices[index++] = x;
			if (m_yUp)
			{
				vertices[index++] = 0;
				vertices[index++] = y;
			}
			else
			{
				vertices[index++] = y;
				vertices[index++] = 0;
			}

			vertices[index++] = 0;
			if (m_yUp)
			{
				vertices[index++] = 1;
				vertices[index++] = 0;
			}
			else
			{
				vertices[index++] = 0;
				vertices[index++] = -1;
			}

			vertices[index++] = 1;
			vertices[index++] = 0;
			vertices[index++] = 0;
			index += skip;

			// add vertex with same position, but with inverted normal & tangent
			if (m_doubleSided)
			{
				for (int k = 0; k < 3; k++)
				{
					vertices[index] = vertices[index - stride];
					++index;
				}
				for (int k = 0; k < 3; k++)
				{
					vertices[index] = -vertices[index - stride];
					++index;
				}
				for (int k = 0; k < 3; k++)
				{
					vertices[index] = -vertices[index - stride];
					++index;
				}
				index += skip;
			}

			if (j != m_segmentsW && i != m_segmentsH)
			{
				base = j + i * tw;
				indices[numIndices++] = base * mult;
				indices[numIndices++] = (base + tw) * mult;
				indices[numIndices++] = (base + tw + 1) * mult;
				indices[numIndices++] = base * mult;
				indices[numIndices++] = (base + tw + 1) * mult;
				indices[numIndices++] = (base + 1) * mult;

				if (m_doubleSided)
				{
					indices[numIndices++] = (base + tw + 1) * mult + 1;
					indices[numIndices++] = (base + tw) * mult + 1;
					indices[numIndices++] = base * mult + 1;
					indices[numIndices++] = (base + 1) * mult + 1;
					indices[numIndices++] = (base + tw + 1) * mult + 1;
					indices[numIndices++] = base * mult + 1;
				}
			}
		}
	}

	target->updateData(vertices, numVertices);
	target->updateIndexData(indices, numIndices);
}

void PlaneGeometry::buildUVs(CompactSubGeometry* target)
{
	float* data = target->getUVData();
	int skip = target->getUVStride() - 2;
	int index = target->getUVOffset();
	for (unsigned short i = 0; i <= m_segmentsH; i++)
	{
		for (unsigned short j = 0; j <= m_segmentsW; j++)
		{
			data[index++] = (float)j / m_segmentsW;
			data[index++] = 1 - (float)i / m_segmentsH;
			index += skip;

			if (m_doubleSided)
			{
				data[index++] = (float)j / m_segmentsW;
				data[index++] = 1 - (float)i / m_segmentsH;
				index += skip;
			}
		}
	}

	target->updateData(data, target->getVertexCount());
}