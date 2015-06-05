#pragma once

#include "Common.h"
#include "SegmentSet.h"

AWAY_NAMESPACE_BEGIN

/**
 * A WireframeSphere primitive mesh
 */
class WireframePrimitiveBase : public SegmentSet
{
public:
	WireframePrimitiveBase(unsigned int color = 0xffffff);

	unsigned int getColor() { return m_color; }
	void setColor(unsigned int value);

	BoundingVolumeBase* getBounds() override;

protected:
	virtual void buildGeometry() = 0;
	void invalidateGeometry();
	void updateOrAddSegment(unsigned int index, Vector3D& v0, Vector3D& v1);
	void updateMouseChildren() override;
	
private:
	void updateGeometry();

private:
	unsigned int m_color;
	bool m_geomDirty;
};

AWAY_NAMESPACE_END