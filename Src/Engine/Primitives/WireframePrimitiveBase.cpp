#include "WireframePrimitiveBase.h"
#include "Segment.h"

USING_AWAY_NAMESPACE

WireframePrimitiveBase::WireframePrimitiveBase(unsigned int color)
{
	m_color = color;
	m_geomDirty = true;
	setMouseEnabled(false);
	setMouseChildren(false);
}

void WireframePrimitiveBase::setColor(unsigned int value)
{
	m_color = value;
	for (auto segment : m_segments)
	{
		segment->setStartColor(value);
		segment->setEndColor(value);
	}
}

BoundingVolumeBase* WireframePrimitiveBase::getBounds()
{
	if (m_geomDirty)
		updateGeometry();

	return SegmentSet::getBounds();
}

void WireframePrimitiveBase::invalidateGeometry()
{
	m_geomDirty = true;
	invalidateBounds();
}

void WireframePrimitiveBase::updateOrAddSegment(unsigned int index, Vector3D& v0, Vector3D& v1)
{
	if (m_segments.size() > index)
	{
		Segment* segment = m_segments[index];
		Vector3D& s = segment->getStart();
		Vector3D& e = segment->getEnd();
		s.m_x = v0.m_x;
		s.m_y = v0.m_y;
		s.m_z = v0.m_z;
		e.m_x = v1.m_x;
		e.m_y = v1.m_y;
		e.m_z = v1.m_z;
		segment->updateSegment(s, e, m_color, m_color);
	}
	else
		addSegment(new Segment(v0, v1, m_color, m_color));
}

void WireframePrimitiveBase::updateMouseChildren()
{
	m_ancestorsAllowMouseEnabled = false;
}

void WireframePrimitiveBase::updateGeometry()
{
	buildGeometry();
	m_geomDirty = false;
}
