#include "TorusGeometry.h"
#include "CompactSubGeometry.h"
#include "MathConsts.h"

USING_AWAY_NAMESPACE

TorusGeometry::TorusGeometry(float radius, float tubeRadius, unsigned short segmentsR, unsigned short segmentsT, bool yUp)
{
	m_radius = radius;
	m_tubeRadius = tubeRadius;
	m_segmentsR = segmentsR;
	m_segmentsT = segmentsT;
	m_yUp = yUp;
}

void TorusGeometry::setRadius(float value)
{
	if (value != m_radius)
	{
		m_radius = value;
		invalidateGeometry();
	}
}

void TorusGeometry::setTubeRadius(float value)
{
	if (value != m_tubeRadius)
	{
		m_tubeRadius = value;
		invalidateGeometry();
	}
}

void TorusGeometry::setSegmentsR(unsigned short value)
{
	if (value != m_segmentsR)
	{
		m_segmentsR = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void TorusGeometry::setSegmentsT(unsigned short value)
{
	if (value != m_segmentsT)
	{
		m_segmentsT = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void TorusGeometry::setYUp(bool value)
{
	if (value != m_yUp)
	{
		m_yUp = value;
		invalidateGeometry();
	}
}

void TorusGeometry::buildGeometry(CompactSubGeometry* target)
{
	int numVertices = (m_segmentsT + 1) * (m_segmentsR + 1);
	int numIndices = m_segmentsT * m_segmentsR * 6;
	int stride = target->getVertexStride();

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

	// evaluate revolution steps
	float revolutionAngleDeltaR = MathConsts::TWO_PI / m_segmentsR;
	float revolutionAngleDeltaT = MathConsts::TWO_PI / m_segmentsT;

	unsigned short vIndex = 0;
	int startIndex, vertexOffset = target->getVertexOffset(), iIndex = 0;
	float revolutionAngleR, revolutionAngleT, length, nx, ny, nz, x, y, z, n1, n2, t1, t2, comp1, comp2;

	// surface
	unsigned short i, j, a, b, c, d;
	for (j = 0; j <= m_segmentsT; j++)
	{
		startIndex = vertexOffset + vIndex * stride;

		for (i = 0; i <= m_segmentsR; i++)
		{
			// revolution vertex
			revolutionAngleR = i * revolutionAngleDeltaR;
			revolutionAngleT = j * revolutionAngleDeltaT;

			length = std::cos(revolutionAngleT);
			nx = length * std::cos(revolutionAngleR);
			ny = length * std::sin(revolutionAngleR);
			nz = std::sin(revolutionAngleT);

			x = m_radius * std::cos(revolutionAngleR) + m_tubeRadius * nx;
			y = m_radius * std::sin(revolutionAngleR) + m_tubeRadius * ny;
			z = (j == m_segmentsT) ? 0 : m_tubeRadius * nz;

			if (m_yUp)
			{
				n1 = -nz;
				n2 = ny;
				t1 = 0;
				t2 = (length ? nx / length : x / m_radius);
				comp1 = -z;
				comp2 = y;
			}
			else
			{
				n1 = ny;
				n2 = nz;
				t1 = (length ? nx / length : x / m_radius);
				t2 = 0;
				comp1 = y;
				comp2 = z;
			}

			if (i == m_segmentsR)
				addVertex(vertices, vertexOffset + vIndex++ * stride, x, vertices[startIndex + 1], vertices[startIndex + 2], nx, n1, n2, -(length ? ny / length : y / m_radius), t1, t2);
			else
				addVertex(vertices, vertexOffset + vIndex++ * stride, x, comp1, comp2, nx, n1, n2, -(length ? ny / length : y / m_radius), t1, t2);

			// close triangle
			if (i > 0 && j > 0)
			{
				a = vIndex - 1; // current
				b = vIndex - 2; // previous
				c = b - m_segmentsR - 1; // previous of last level
				d = a - m_segmentsR - 1; // current of last level
				addTriangleClockWise(indices, iIndex, a, b, c);
				addTriangleClockWise(indices, iIndex, a, c, d);
			}
		}
	}

	target->updateData(vertices, numVertices);
	target->updateIndexData(indices, numIndices);
}

void TorusGeometry::buildUVs(CompactSubGeometry* target)
{
	float* data = target->getUVData();
	int skip = target->getUVStride() - 2;
	int index = target->getUVOffset();
	for (unsigned short j = 0; j <= m_segmentsT; j++)
	{
		for (unsigned short i = 0; i <= m_segmentsR; i++)
		{
			// revolution vertex
			data[index++] = (float)i / m_segmentsR;
			data[index++] = (float)j / m_segmentsT;
			index += skip;
		}
	}

	target->updateData(data, target->getVertexCount());
}

void TorusGeometry::addVertex(float* vertices, int vIndex, float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz)
{
	vertices[vIndex++] = px;
	vertices[vIndex++] = py;
	vertices[vIndex++] = pz;
	vertices[vIndex++] = nx;
	vertices[vIndex++] = ny;
	vertices[vIndex++] = nz;
	vertices[vIndex++] = tx;
	vertices[vIndex++] = ty;
	vertices[vIndex] = tz;
}

void TorusGeometry::addTriangleClockWise(unsigned short* indices, int& iIndex, unsigned short index0, unsigned short index1, unsigned short index2)
{
	indices[iIndex++] = index0;
	indices[iIndex++] = index1;
	indices[iIndex++] = index2;
}