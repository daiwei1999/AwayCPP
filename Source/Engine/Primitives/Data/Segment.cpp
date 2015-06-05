#include "Segment.h"
#include "SegmentSet.h"

USING_AWAY_NAMESPACE

Segment::Segment(Vector3D& start, Vector3D& end, unsigned int colorStart, unsigned int colorEnd)
{
	m_segmentSet = nullptr;
	m_start.copyFrom(start);
	m_end.copyFrom(end);
	setStartColor(colorStart);
	setEndColor(colorEnd);
}

void Segment::updateSegment(Vector3D& start, Vector3D& end, unsigned int colorStart, unsigned int colorEnd)
{
	m_start.copyFrom(start);
	m_end.copyFrom(end);
	setStartColor(colorStart);
	setEndColor(colorEnd);
	update();
}

void Segment::setStart(Vector3D& value)
{
	m_start.copyFrom(value);
	update();
}

void Segment::setEnd(Vector3D& value)
{
	m_end.copyFrom(value);
	update();
}

void Segment::setStartColor(unsigned int value)
{
	m_startR = ((value >> 16) & 0xff) / 255.f;
	m_startG = ((value >> 8) & 0xff) / 255.f;
	m_startB = (value & 0xff) / 255.f;
	m_startColor = value;
	update();
}

void Segment::setEndColor(unsigned int value)
{
	m_endR = ((value >> 16) & 0xff) / 255.f;
	m_endG = ((value >> 8) & 0xff) / 255.f;
	m_endB = (value & 0xff) / 255.f;
	m_endColor = value;
	update();
}

void Segment::update()
{
	if (m_segmentSet)
		m_segmentSet->updateSegment(this);
}