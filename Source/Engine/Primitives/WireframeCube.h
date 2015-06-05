#pragma once

#include "Common.h"
#include "WireframePrimitiveBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * A WirefameCube primitive mesh.
 */
class WireframeCube : public WireframePrimitiveBase
{
public:
	WireframeCube(float width = 100, float height = 100, float depth = 100, unsigned int color = 0xFFFFFF);

	float getWidth() { return m_width; }
	void setWidth(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
	float getDepth() { return m_depth; }
	void setDepth(float value);

protected:
	void buildGeometry() override;

private:
	float m_width;
	float m_height;
	float m_depth;
};

AWAY_NAMESPACE_END