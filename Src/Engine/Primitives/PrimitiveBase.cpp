#include "PrimitiveBase.h"
#include "CompactSubGeometry.h"

USING_AWAY_NAMESPACE

PrimitiveBase::PrimitiveBase()
{
	m_geomDirty = true;
	m_uvDirty = true;
	m_subGeometry = new CompactSubGeometry();
	addSubGeometry(m_subGeometry);
}

SubGeometryVector& PrimitiveBase::getSubGeometries()
{
	if (m_geomDirty)
		updateGeometry();

	if (m_uvDirty)
		updateUVs();

	return Geometry::getSubGeometries();
}

void PrimitiveBase::updateGeometry()
{
	buildGeometry(m_subGeometry);
	m_geomDirty = false;
}

void PrimitiveBase::updateUVs()
{
	buildUVs(m_subGeometry);
	m_uvDirty = false;
}