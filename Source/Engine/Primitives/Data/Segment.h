#pragma once

#include "Common.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

class Segment
{
public:
	Segment(Vector3D& start, Vector3D& end, unsigned int colorStart = 0x333333, unsigned int colorEnd = 0x333333);

	void updateSegment(Vector3D& start, Vector3D& end, unsigned int colorStart = 0x333333, unsigned int colorEnd = 0x333333);
	Vector3D& getStart() { return m_start; }
	void setStart(Vector3D& value);
	Vector3D& getEnd() { return m_end; }
	void setEnd(Vector3D& value);
	unsigned int getStartColor() { return m_startColor; }
	void setStartColor(unsigned int value);
	unsigned int getEndColor() { return m_endColor; }
	void setEndColor(unsigned int value);
	unsigned int getIndex() { return m_index; }
	void setIndex(unsigned int value) { m_index = value; }
	void setSegmentSet(SegmentSet* value) { m_segmentSet = value; }

private:
	void update();

	friend class SegmentSet;
private:
	SegmentSet* m_segmentSet;
	int m_index;
	Vector3D m_start, m_end;
	unsigned int m_startColor;
	unsigned int m_endColor;
	float m_startR, m_startG, m_startB;
	float m_endR, m_endG, m_endB;
};

AWAY_NAMESPACE_END