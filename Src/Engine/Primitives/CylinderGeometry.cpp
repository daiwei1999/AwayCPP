#include "CylinderGeometry.h"
#include "CompactSubGeometry.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

CylinderGeometry::CylinderGeometry(float topRadius, float bottomRadius, float height, unsigned short segmentsW, unsigned short segmentsH, bool topClosed, bool bottomClosed, bool surfaceClosed, bool yUp)
{
	m_topRadius = topRadius;
	m_bottomRadius = bottomRadius;
	m_height = height;
	m_segmentsW = segmentsW;
	m_segmentsH = segmentsH;
	m_topClosed = topClosed;
	m_bottomClosed = bottomClosed;
	m_surfaceClosed = surfaceClosed;
	m_yUp = yUp;
}

void CylinderGeometry::setTopRadius(float value)
{
	if (value != m_topRadius)
	{
		m_topRadius = value;
		invalidateGeometry();
	}
}

void CylinderGeometry::setBottomRadius(float value)
{
	if (value != m_bottomRadius)
	{
		m_bottomRadius = value;
		invalidateGeometry();
	}
}

void CylinderGeometry::setHeight(float value)
{
	if (value != m_height)
	{
		m_height = value;
		invalidateGeometry();
	}
}

void CylinderGeometry::setSegmentsW(unsigned short value)
{
	if (value != m_segmentsW)
	{
		m_segmentsW = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void CylinderGeometry::setSegmentsH(unsigned short value)
{
	if (value != m_segmentsH)
	{
		m_segmentsH = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void CylinderGeometry::setTopClosed(bool value)
{
	if (value != m_topClosed)
	{
		m_topClosed = value;
		invalidateGeometry();
	}
}

void CylinderGeometry::setBottomClosed(bool value)
{
	if (value != m_bottomClosed)
	{
		m_bottomClosed = value;
		invalidateGeometry();
	}
}

void CylinderGeometry::setYUp(bool value)
{
	if (value != m_yUp)
	{
		m_yUp = value;
		invalidateGeometry();
	}
}

void CylinderGeometry::buildGeometry(CompactSubGeometry* target)
{
	int numVertices = 0, numTriangles = 0;
	int stride = target->getVertexStride();
	if (m_surfaceClosed)
	{
		numVertices += (m_segmentsH + 1) * (m_segmentsW + 1); // segmentsH + 1 because of closure, segmentsW + 1 because of UV unwrapping
		numTriangles += m_segmentsH * m_segmentsW * 2; // each level has segmentW quads, each of 2 triangles
	}
	if (m_topClosed)
	{
		numVertices += 2 * (m_segmentsW + 1); // segmentsW + 1 because of unwrapping
		numTriangles += m_segmentsW; // one triangle for each segment
	}
	if (m_bottomClosed)
	{
		numVertices += 2 * (m_segmentsW + 1);
		numTriangles += m_segmentsW;
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
		indices = new unsigned short[numTriangles * 3];
	}

	// evaluate revolution steps
	float revolutionAngleDelta = MathConsts::TWO_PI / m_segmentsW;

	// top
	unsigned short i, j;
	int vertexOffset = target->getVertexOffset();
	int startIndex = 0, nextVertexIndex = 0, currentIndex = 0;
	float x, y, z, revolutionAngle, comp1, comp2, t1, t2;
	if (m_topClosed && m_topRadius > 0)
	{
		z = -0.5f * m_height;

		for (i = 0; i <= m_segmentsW; i++)
		{
			// central vertex
			if (m_yUp)
			{
				t1 = 1;
				t2 = 0;
				comp1 = -z;
				comp2 = 0;
			}
			else
			{
				t1 = 0;
				t2 = -1;
				comp1 = 0;
				comp2 = z;
			}

			addVertex(vertices, vertexOffset, nextVertexIndex, stride, 0, comp1, comp2, 0, t1, t2, 1, 0, 0);

			// revolution vertex
			revolutionAngle = i * revolutionAngleDelta;
			x = m_topRadius * std::cos(revolutionAngle);
			y = m_topRadius * std::sin(revolutionAngle);

			if (m_yUp)
			{
				comp1 = -z;
				comp2 = y;
			}
			else
			{
				comp1 = y;
				comp2 = z;
			}

			if (i == m_segmentsW)
				addVertex(vertices, vertexOffset, nextVertexIndex, stride, vertices[startIndex + stride], vertices[startIndex + stride + 1], vertices[startIndex + stride + 2], 0, t1, t2, 1, 0, 0);
			else
				addVertex(vertices, vertexOffset, nextVertexIndex, stride, x, comp1, comp2, 0, t1, t2, 1, 0, 0);

			if (i > 0) // add triangle
				addTriangleClockWise(indices, currentIndex, nextVertexIndex - 1, nextVertexIndex - 3, nextVertexIndex - 2);
		}
	}

	// bottom
	if (m_bottomClosed && m_bottomRadius > 0)
	{
		z = 0.5f * m_height;

		startIndex = vertexOffset + nextVertexIndex * stride;

		for (i = 0; i <= m_segmentsW; i++)
		{
			if (m_yUp)
			{
				t1 = -1;
				t2 = 0;
				comp1 = -z;
				comp2 = 0;
			}
			else
			{
				t1 = 0;
				t2 = 1;
				comp1 = 0;
				comp2 = z;
			}

			addVertex(vertices, vertexOffset, nextVertexIndex, stride, 0, comp1, comp2, 0, t1, t2, 1, 0, 0);

			// revolution vertex
			revolutionAngle = i * revolutionAngleDelta;
			x = m_bottomRadius * std::cos(revolutionAngle);
			y = m_bottomRadius * std::sin(revolutionAngle);

			if (m_yUp)
			{
				comp1 = -z;
				comp2 = y;
			}
			else
			{
				comp1 = y;
				comp2 = z;
			}

			if (i == m_segmentsW)
				addVertex(vertices, vertexOffset, nextVertexIndex, stride, x, vertices[startIndex + 1], vertices[startIndex + 2], 0, t1, t2, 1, 0, 0);
			else
				addVertex(vertices, vertexOffset, nextVertexIndex, stride, x, comp1, comp2, 0, t1, t2, 1, 0, 0);

			if (i > 0) // add triangle
				addTriangleClockWise(indices, currentIndex, nextVertexIndex - 2, nextVertexIndex - 3, nextVertexIndex - 1);
		}
	}

	// The normals on the lateral surface all have the same incline, i.e.
	// the "elevation" component (Y or Z depending on yUp) is constant.
	// Same principle goes for the "base" of these vectors, which will be
	// calculated such that a vector [base,elev] will be a unit vector.
	float dr = (m_bottomRadius - m_topRadius);
	float latNormElev = dr / m_height;
	float latNormBase = (latNormElev == 0) ? 1 : m_height / dr;
	float radius, na0, na1, naComp1, naComp2;

	// lateral surface
	if (m_surfaceClosed)
	{
		for (j = 0; j <= m_segmentsH; j++)
		{
			radius = m_topRadius - (((float)j / m_segmentsH) * (m_topRadius - m_bottomRadius));
			z = -(m_height / 2) + ((float)j / m_segmentsH * m_height);

			startIndex = vertexOffset + nextVertexIndex * stride;

			for (i = 0; i <= m_segmentsW; i++)
			{
				// revolution vertex
				revolutionAngle = i * revolutionAngleDelta;
				x = radius * std::cos(revolutionAngle);
				y = radius * std::sin(revolutionAngle);
				na0 = latNormBase * std::cos(revolutionAngle);
				na1 = latNormBase * std::sin(revolutionAngle);

				if (m_yUp)
				{
					t1 = 0;
					t2 = -na0;
					comp1 = -z;
					comp2 = y;
					naComp1 = latNormElev;
					naComp2 = na1;
				}
				else
				{
					t1 = -na0;
					t2 = 0;
					comp1 = y;
					comp2 = z;
					naComp1 = na1;
					naComp2 = latNormElev;
				}

				if (i == m_segmentsW)
					addVertex(vertices, vertexOffset, nextVertexIndex, stride, vertices[startIndex], vertices[startIndex + 1], vertices[startIndex + 2], na0, latNormElev, na1, na1, t1, t2);
				else
					addVertex(vertices, vertexOffset, nextVertexIndex, stride, x, comp1, comp2, na0, naComp1, naComp2, -na1, t1, t2);

				// close triangle
				if (i > 0 && j > 0)
				{
					int a = nextVertexIndex - 1; // current
					int b = nextVertexIndex - 2; // previous
					int c = b - m_segmentsW - 1; // previous of last level
					int d = a - m_segmentsW - 1; // current of last level
					addTriangleClockWise(indices, currentIndex, a, b, c);
					addTriangleClockWise(indices, currentIndex, a, c, d);
				}
			}
		}
	}

	target->updateData(vertices, numVertices);
	target->updateIndexData(indices, currentIndex);
}

void CylinderGeometry::buildUVs(CompactSubGeometry* target)
{
	float* data = target->getUVData();
	int skip = target->getUVStride() - 2;
	int index = target->getUVOffset();
	float revolutionAngle, revolutionAngleDelta = MathConsts::TWO_PI / m_segmentsW;
	unsigned short i, j;

	// top
	if (m_topClosed)
	{
		for (i = 0; i <= m_segmentsW; i++)
		{
			revolutionAngle = i * revolutionAngleDelta;
			data[index++] = 0.5f; // central vertex
			data[index++] = 0.5f;
			index += skip;
			data[index++] = 0.5f - 0.5f * std::cos(revolutionAngle); // revolution vertex
			data[index++] = 0.5f + 0.5f * std::sin(revolutionAngle);
			index += skip;
		}
	}

	// bottom
	if (m_bottomClosed)
	{
		for (i = 0; i <= m_segmentsW; i++)
		{
			revolutionAngle = i * revolutionAngleDelta;
			data[index++] = 0.5f; // central vertex
			data[index++] = 0.5f;
			index += skip;
			data[index++] = 0.5f + 0.5f * std::cos(revolutionAngle); // revolution vertex
			data[index++] = 0.5f + 0.5f * std::sin(revolutionAngle);
			index += skip;
		}
	}

	// lateral surface
	if (m_surfaceClosed)
	{
		for (j = 0; j <= m_segmentsH; j++)
		{
			for (i = 0; i <= m_segmentsW; i++)
			{
				// revolution vertex
				data[index++] = (float)i / m_segmentsW;
				data[index++] = (float)j / m_segmentsH;
				index += skip;
			}
		}
	}

	target->updateData(data, target->getVertexCount());
}

void CylinderGeometry::addVertex(float* data, int vertexOffset, int& nextVertexIndex, int stride, float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz)
{
	int index = vertexOffset + nextVertexIndex++ * stride;
	data[index++] = px;
	data[index++] = py;
	data[index++] = pz;
	data[index++] = nx;
	data[index++] = ny;
	data[index++] = nz;
	data[index++] = tx;
	data[index++] = ty;
	data[index++] = tz;
}

void CylinderGeometry::addTriangleClockWise(unsigned short* data, int& index, int index0, int index1, int index2)
{
	data[index++] = (unsigned short)index0;
	data[index++] = (unsigned short)index1;
	data[index++] = (unsigned short)index2;
}