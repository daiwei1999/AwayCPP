#pragma once

#include "Common.h"
#include "Geometry.h"

AWAY_NAMESPACE_BEGIN

class PrimitiveBase : public Geometry
{
public:
	PrimitiveBase();

	SubGeometryVector& getSubGeometries() override;

protected:
	void invalidateGeometry() { m_geomDirty = true; }
	void invalidateUVs() { m_uvDirty = true; }
	virtual void buildGeometry(CompactSubGeometry* target) = 0;
	virtual void buildUVs(CompactSubGeometry* target) = 0;

private:
	void updateGeometry();
	void updateUVs();

protected:
	bool m_geomDirty;
	bool m_uvDirty;

private:
	CompactSubGeometry* m_subGeometry;
};

AWAY_NAMESPACE_END