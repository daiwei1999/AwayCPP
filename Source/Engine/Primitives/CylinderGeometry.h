#pragma once

#include "Common.h"
#include "PrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

class CylinderGeometry : public PrimitiveBase
{
public:
	CylinderGeometry(float topRadius = 50, float bottomRadius = 50, float height = 100, unsigned short segmentsW = 16, unsigned short segmentsH = 1, bool topClosed = true, bool bottomClosed = true, bool surfaceClosed = true, bool yUp = true);

	float getTopRadius() { return m_topRadius; }
	void setTopRadius(float value);
	float getBottomRadius() { return m_bottomRadius; }
	void setBottomRadius(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
	unsigned short getSegmentsW() { return m_segmentsW; }
	void setSegmentsW(unsigned short value);
	unsigned short getSegmentsH() { return m_segmentsH; }
	void setSegmentsH(unsigned short value);
	bool getTopClosed() { return m_topClosed; }
	void setTopClosed(bool value);
	bool getBottomClosed() { return m_bottomClosed; }
	void setBottomClosed(bool value);
	bool getYUp() { return m_yUp; }
	void setYUp(bool value);

protected:
	void buildGeometry(CompactSubGeometry* target) override;
	void buildUVs(CompactSubGeometry* target) override;

private:
	void addVertex(float* data, int vertexOffset, int& nextVertexIndex, int stride, float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz);
	void addTriangleClockWise(unsigned short* data, int& index, int index0, int index1, int index2);

protected:
	float m_topRadius;
	float m_bottomRadius;
	float m_height;
	unsigned short m_segmentsW;
	unsigned short m_segmentsH;
	bool m_topClosed;
	bool m_bottomClosed;
	bool m_surfaceClosed;
	bool m_yUp;
};

AWAY_NAMESPACE_END