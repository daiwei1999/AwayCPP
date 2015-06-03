#pragma once

#include "Common.h"
#include "PrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class TorusGeometry : public PrimitiveBase
{
public:
	TorusGeometry(float radius = 50, float tubeRadius = 50, unsigned short segmentsR = 16, unsigned short segmentsT = 8, bool yUp = true);

	float getRadius() { return m_radius; }
	void setRadius(float value);
	float getTubeRadius() { return m_tubeRadius; }
	void setTubeRadius(float value);
	unsigned short getSegmentsR() { return m_segmentsR; }
	void setSegmentsR(unsigned short value);
	unsigned short getSegmentsT() { return m_segmentsT; }
	void setSegmentsT(unsigned short value);
	bool getYUp() { return m_yUp; }
	void setYUp(bool value);

protected:
	void buildGeometry(CompactSubGeometry* target) override;
	void buildUVs(CompactSubGeometry* target) override;

private:
	void addVertex(float* vertices, int vIndex, float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz);
	void addTriangleClockWise(unsigned short* indices, int& iIndex, unsigned short index0, unsigned short index1, unsigned short index2);

private:
	float m_radius;
	float m_tubeRadius;
	unsigned short m_segmentsR;
	unsigned short m_segmentsT;
	bool m_yUp;
};

AWAY_NAMESPACE_END