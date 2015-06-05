#include "WireframeSphere.h"

USING_AWAY_NAMESPACE

WireframeSphere::WireframeSphere(float radius, int segmentsW, int segmentsH, unsigned int color) : WireframePrimitiveBase(color)
{
	m_radius = radius;
	m_segmentsW = segmentsW;
	m_segmentsH = segmentsH;
}

void WireframeSphere::setRadius(float value)
{
	m_radius = value;
	invalidateGeometry();
}

void WireframeSphere::setSegmentsW(int value)
{
	m_segmentsW = value;
	removeAllSegments();
	invalidateGeometry();
}

void WireframeSphere::setSegmentsH(int value)
{
	m_segmentsH = value;
	removeAllSegments();
	invalidateGeometry();
}

void WireframeSphere::buildGeometry()
{
	std::vector<float> vertices;
	float horAngle, y, ringRadius;
	float horAngleDelta = MathConsts::PI / m_segmentsH;
	float verAngleDelta = MathConsts::TWO_PI / m_segmentsW;

	for (int j = 0; j <= m_segmentsH; j++)
	{
		horAngle = j * horAngleDelta;
		y = m_radius * std::cos(horAngle);
		ringRadius = m_radius * std::sin(horAngle);

		for (int i = 0; i <= m_segmentsW; i++)
		{
			float verangle = i * verAngleDelta;
			vertices.push_back(ringRadius * std::cos(verangle));
			vertices.push_back(y);
			vertices.push_back(ringRadius * std::sin(verangle));
		}
	}

	Vector3D v0, v1;
	int a, b, c, d, index = 0;
	for (int j = 1; j <= m_segmentsH; j++)
	{
		for (int i = 1; i <= m_segmentsW; i++)
		{
			a = ((m_segmentsW + 1) * j + i) * 3;
			b = ((m_segmentsW + 1) * j + i - 1) * 3;
			c = ((m_segmentsW + 1) * (j - 1) + i - 1) * 3;
			d = ((m_segmentsW + 1) * (j - 1) + i) * 3;

			if (j == m_segmentsH)
			{
				v0.m_x = vertices[c];
				v0.m_y = vertices[c + 1];
				v0.m_z = vertices[c + 2];
				v1.m_x = vertices[d];
				v1.m_y = vertices[d + 1];
				v1.m_z = vertices[d + 2];
				updateOrAddSegment(index++, v0, v1);
				v0.m_x = vertices[a];
				v0.m_y = vertices[a + 1];
				v0.m_z = vertices[a + 2];
				updateOrAddSegment(index++, v0, v1);
			}
			else if (j == 1)
			{
				v1.m_x = vertices[b];
				v1.m_y = vertices[b + 1];
				v1.m_z = vertices[b + 2];
				v0.m_x = vertices[c];
				v0.m_y = vertices[c + 1];
				v0.m_z = vertices[c + 2];
				updateOrAddSegment(index++, v0, v1);
			}
			else
			{
				v1.m_x = vertices[b];
				v1.m_y = vertices[b + 1];
				v1.m_z = vertices[b + 2];
				v0.m_x = vertices[c];
				v0.m_y = vertices[c + 1];
				v0.m_z = vertices[c + 2];
				updateOrAddSegment(index++, v0, v1);
				v1.m_x = vertices[d];
				v1.m_y = vertices[d + 1];
				v1.m_z = vertices[d + 2];
				updateOrAddSegment(index++, v0, v1);
			}
		}
	}
}