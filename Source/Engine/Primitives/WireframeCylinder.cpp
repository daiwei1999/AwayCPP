#include "WireframeCylinder.h"

USING_AWAY_NAMESPACE

WireframeCylinder::WireframeCylinder(float topRadius, float bottomRadius, float height, int segmentsW, int segmentsH, unsigned int color) : WireframePrimitiveBase(color)
{
	m_topRadius = topRadius;
	m_bottomRadius = bottomRadius;
	m_height = height;
	m_segmentsW = segmentsW;
	m_segmentsH = segmentsH;
}

void WireframeCylinder::setTopRadius(float value)
{
	m_topRadius = value;
	invalidateGeometry();
}

void WireframeCylinder::setBottomRadius(float value)
{
	m_bottomRadius = value;
	invalidateGeometry();
}

void WireframeCylinder::setHeight(float value)
{
	m_height = value;
	invalidateGeometry();
}

void WireframeCylinder::buildGeometry()
{
	int nextVertexIndex = 0;
	float x, y, z, radius, revolutionAngle;
	float revolutionAngleDelta = MathConsts::TWO_PI / m_segmentsW;

	Vector3D previousV, currentV;
	std::vector<std::vector<Vector3D>> lastLayer(m_segmentsH + 1);

	for (int j = 0; j <= m_segmentsH; j++)
	{
		radius = m_topRadius - j * (m_topRadius - m_bottomRadius) / m_segmentsH;
		y = m_height * 0.5f - j * m_height / m_segmentsH;

		for (int i = 0; i <= m_segmentsW; i++)
		{
			revolutionAngle = i * revolutionAngleDelta;
			x = radius * std::cos(revolutionAngle);
			z = radius * std::sin(revolutionAngle);

			if (i == 0)
				previousV.setTo(x, y, z);
			else
			{
				currentV.setTo(x, y, z);
				updateOrAddSegment(nextVertexIndex++, previousV, currentV);
				previousV.copyFrom(currentV);

				if (j > 0)
					updateOrAddSegment(nextVertexIndex++, currentV, lastLayer[j - 1][i]);
			}

			lastLayer[j].push_back(previousV);
		}
	}
}