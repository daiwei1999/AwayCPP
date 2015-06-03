#include "CubeGeometry.h"
#include "CompactSubGeometry.h"

USING_AWAY_NAMESPACE

CubeGeometry::CubeGeometry(float width, float height, float depth, unsigned short segmentsW, unsigned short segmentsH, unsigned short segmentsD, bool tile6)
{
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_segmentsW = segmentsW;
	m_segmentsH = segmentsH;
	m_segmentsD = segmentsD;
	m_tile6 = tile6;
}

void CubeGeometry::setWidth(float value)
{
	if (value != m_width)
	{
		m_width = value;
		invalidateGeometry();
	}
}

void CubeGeometry::setHeight(float value)
{
	if (value != m_height)
	{
		m_height = value;
		invalidateGeometry();
	}
}

void CubeGeometry::setDepth(float value)
{
	if (value != m_depth)
	{
		m_depth = value;
		invalidateGeometry();
	}
}

void CubeGeometry::setTile6(bool value)
{
	if (value != m_tile6)
	{
		m_tile6 = value;
		invalidateUVs();
	}
}

void CubeGeometry::setSegmentsW(unsigned short value)
{
	if (value != m_segmentsW)
	{
		m_segmentsW = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void CubeGeometry::setSegmentsH(unsigned short value)
{
	if (value != m_segmentsH)
	{
		m_segmentsH = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void CubeGeometry::setSegmentsD(unsigned short value)
{
	if (value != m_segmentsD)
	{
		m_segmentsD = value;
		invalidateGeometry();
		invalidateUVs();
	}
}

void CubeGeometry::buildGeometry(CompactSubGeometry* target)
{
	int numVertices = ((m_segmentsW + 1) * (m_segmentsH + 1) + (m_segmentsW + 1) * (m_segmentsD + 1) + (m_segmentsH + 1) * (m_segmentsD + 1)) * 2;
	int numIndices = (m_segmentsW * m_segmentsH + m_segmentsW * m_segmentsD + m_segmentsH * m_segmentsD) * 12;
	int stride = target->getVertexStride();
	int skip = stride - 9;

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

	// half cube dimensions
	float hw = m_width / 2;
	float hh = m_height / 2;
	float hd = m_depth / 2;

	// Segment dimensions
	float dw = m_width / m_segmentsW;
	float dh = m_height / m_segmentsH;
	float dd = m_depth / m_segmentsD;

	float outerPos;
	unsigned short i, j, tl, tr, bl, br;
	int vidx = target->getVertexOffset(), fidx = 0;
	for (i = 0; i <= m_segmentsW; i++)
	{
		outerPos = -hw + i * dw;
		for (j = 0; j <= m_segmentsH; j++)
		{
			// front
			vertices[vidx++] = outerPos;
			vertices[vidx++] = -hh + j * dh;
			vertices[vidx++] = -hd;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = -1;
			vertices[vidx++] = 1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vidx += skip;

			// back
			vertices[vidx++] = outerPos;
			vertices[vidx++] = -hh + j * dh;
			vertices[vidx++] = hd;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = 1;
			vertices[vidx++] = -1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vidx += skip;

			if (i > 0 && j > 0)
			{
				tl = 2 * ((i - 1) * (m_segmentsH + 1) + (j - 1));
				tr = 2 * (i * (m_segmentsH + 1) + (j - 1));
				bl = tl + 2;
				br = tr + 2;

				indices[fidx++] = tl;
				indices[fidx++] = bl;
				indices[fidx++] = br;
				indices[fidx++] = tl;
				indices[fidx++] = br;
				indices[fidx++] = tr;
				indices[fidx++] = tr + 1;
				indices[fidx++] = br + 1;
				indices[fidx++] = bl + 1;
				indices[fidx++] = tr + 1;
				indices[fidx++] = bl + 1;
				indices[fidx++] = tl + 1;
			}
		}
	}

	unsigned short inc = 2 * (m_segmentsW + 1) * (m_segmentsH + 1);
	for (i = 0; i <= m_segmentsW; i++)
	{
		outerPos = -hw + i * dw;
		for (j = 0; j <= m_segmentsD; j++)
		{
			// top
			vertices[vidx++] = outerPos;
			vertices[vidx++] = hh;
			vertices[vidx++] = -hd + j * dd;
			vertices[vidx++] = 0;
			vertices[vidx++] = 1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vidx += skip;

			// bottom
			vertices[vidx++] = outerPos;
			vertices[vidx++] = -hh;
			vertices[vidx++] = -hd + j * dd;
			vertices[vidx++] = 0;
			vertices[vidx++] = -1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vidx += skip;

			if (i > 0 && j > 0)
			{
				tl = inc + 2 * ((i - 1) * (m_segmentsD + 1) + (j - 1));
				tr = inc + 2 * (i * (m_segmentsD + 1) + (j - 1));
				bl = tl + 2;
				br = tr + 2;

				indices[fidx++] = tl;
				indices[fidx++] = bl;
				indices[fidx++] = br;
				indices[fidx++] = tl;
				indices[fidx++] = br;
				indices[fidx++] = tr;
				indices[fidx++] = tr + 1;
				indices[fidx++] = br + 1;
				indices[fidx++] = bl + 1;
				indices[fidx++] = tr + 1;
				indices[fidx++] = bl + 1;
				indices[fidx++] = tl + 1;
			}
		}
	}

	inc += 2 * (m_segmentsW + 1) * (m_segmentsD + 1);
	for (i = 0; i <= m_segmentsD; i++)
	{
		outerPos = hd - i * dd;
		for (j = 0; j <= m_segmentsH; j++)
		{
			// left
			vertices[vidx++] = -hw;
			vertices[vidx++] = -hh + j * dh;
			vertices[vidx++] = outerPos;
			vertices[vidx++] = -1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = -1;
			vidx += skip;

			// right
			vertices[vidx++] = hw;
			vertices[vidx++] = -hh + j * dh;
			vertices[vidx++] = outerPos;
			vertices[vidx++] = 1;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = 0;
			vertices[vidx++] = 1;
			vidx += skip;

			if (i > 0 && j > 0)
			{
				tl = inc + 2 * ((i - 1) * (m_segmentsH + 1) + (j - 1));
				tr = inc + 2 * (i * (m_segmentsH + 1) + (j - 1));
				bl = tl + 2;
				br = tr + 2;

				indices[fidx++] = tl;
				indices[fidx++] = bl;
				indices[fidx++] = br;
				indices[fidx++] = tl;
				indices[fidx++] = br;
				indices[fidx++] = tr;
				indices[fidx++] = tr + 1;
				indices[fidx++] = br + 1;
				indices[fidx++] = bl + 1;
				indices[fidx++] = tr + 1;
				indices[fidx++] = bl + 1;
				indices[fidx++] = tl + 1;
			}
		}
	}

	target->updateData(vertices, numVertices);
	target->updateIndexData(indices, numIndices);
}

void CubeGeometry::buildUVs(CompactSubGeometry* target)
{
	float u_tile_dim, v_tile_dim, u_tile_step, v_tile_step;
	if (m_tile6)
	{
		u_tile_dim = u_tile_step = 1 / 3.f;
		v_tile_dim = v_tile_step = 0.5f;
	}
	else
	{
		u_tile_dim = v_tile_dim = 1;
		u_tile_step = v_tile_step = 0;
	}

	// Create planes two and two, the same way that they were
	// constructed in the buildGeometry() function. First calculate
	// the top-left UV coordinate for both planes, and then loop
	// over the points, calculating the UVs from these numbers.

	// When tile6 is true, the layout is as follows:
	//       .-----.-----.-----. (1,1)
	//       | Bot |  T  | Bak |
	//       |-----+-----+-----|
	//       |  L  |  F  |  R  |
	// (0,0)'-----'-----'-----'
	float* data = target->getUVData();
	int skip = target->getUVStride() - 2;
	int uidx = target->getUVOffset();

	// FRONT / BACK
	unsigned short i, j;
	float tl0u = u_tile_step;
	float tl0v = v_tile_step;
	float tl1u = 2 * u_tile_step;
	float tl1v = 0;
	float du = u_tile_dim / m_segmentsW;
	float dv = v_tile_dim / m_segmentsH;
	for (i = 0; i <= m_segmentsW; i++)
	{
		for (j = 0; j <= m_segmentsH; j++)
		{
			data[uidx++] = tl0u + i * du;
			data[uidx++] = tl0v + (v_tile_dim - j * dv);
			uidx += skip;
			data[uidx++] = tl1u + (u_tile_dim - i * du);
			data[uidx++] = tl1v + (v_tile_dim - j * dv);
			uidx += skip;
		}
	}

	// TOP / BOTTOM
	tl0u = u_tile_step;
	du = u_tile_dim / m_segmentsW;
	dv = v_tile_dim / m_segmentsD;
	for (i = 0; i <= m_segmentsW; i++)
	{
		for (j = 0; j <= m_segmentsD; j++)
		{
			data[uidx++] = tl0u + i * du;
			data[uidx++] = v_tile_dim - j * dv;
			uidx += skip;
			data[uidx++] = i * du;
			data[uidx++] = j * dv;
			uidx += skip;
		}
	}

	// LEFT / RIGHT
	tl0v = v_tile_step;
	tl1u = 2 * u_tile_step;
	tl1v = v_tile_step;
	du = u_tile_dim / m_segmentsD;
	dv = v_tile_dim / m_segmentsH;
	for (i = 0; i <= m_segmentsD; i++)
	{
		for (j = 0; j <= m_segmentsH; j++)
		{
			data[uidx++] = i * du;
			data[uidx++] = tl0v + (v_tile_dim - j * dv);
			uidx += skip;
			data[uidx++] = tl1u + (u_tile_dim - i * du);
			data[uidx++] = tl1v + (v_tile_dim - j * dv);
			uidx += skip;
		}
	}

	target->updateData(data, target->getVertexCount());
}