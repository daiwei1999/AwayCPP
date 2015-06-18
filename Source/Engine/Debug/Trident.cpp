#include "Trident.h"
#include "Segment.h"

USING_AWAY_NAMESPACE

Trident::Trident(float length, bool showLetters)
{
	buildTrident(length, showLetters);
}

void Trident::buildTrident(float length, bool showLetters)
{
	std::vector<unsigned int> colors = { 0xFF0000, 0x00FF00, 0x0000FF };
	std::vector<std::vector<Vector3D>> vectors = { { Vector3D::ZERO, Vector3D(length, 0, 0) }, { Vector3D::ZERO, Vector3D(0, length, 0) }, { Vector3D::ZERO, Vector3D(0, 0, length) } };

	if (showLetters)
	{
		float scaleH = length / 10;
		float scaleW = length / 20;
		float scl1 = scaleW * 1.5f;
		float scl2 = scaleH * 3;
		float scl3 = scaleH * 2;
		float scl4 = scaleH * 3.4f;
		float cross = length + scl3 + (scl4 - scl3) / 3 * 2;
		colors.push_back(0xFF0000);
		colors.push_back(0x00FF00);
		colors.push_back(0x0000FF);
		vectors.push_back({ Vector3D(length + scl2, scl1, 0), Vector3D(length + scl3, -scl1, 0), Vector3D(length + scl3, scl1, 0), Vector3D(length + scl2, -scl1, 0) });
		vectors.push_back({ Vector3D(-scaleW * 1.2f, length + scl4, 0), Vector3D(0, cross, 0), Vector3D(scaleW * 1.2f, length + scl4, 0), Vector3D(0, cross, 0), Vector3D(0, cross, 0), Vector3D(0, length + scl3, 0) });
		vectors.push_back({ Vector3D(0, scl1, length + scl2), Vector3D(0, scl1, length + scl3), Vector3D(0, -scl1, length + scl2), Vector3D(0, -scl1, length + scl3), Vector3D(0, -scl1, length + scl3), Vector3D(0, scl1, length + scl2) });
	}

	for (size_t i = 0; i < vectors.size(); i++)
	{
		unsigned int color = colors[i];
		std::vector<Vector3D>& letter = vectors[i];

		for (size_t j = 0; j < letter.size(); j += 2)
			addSegment(new Segment(letter[j], letter[j + 1], color, color));
	}
}