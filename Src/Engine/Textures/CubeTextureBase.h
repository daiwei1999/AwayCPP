#pragma once

#include "Common.h"
#include "TextureProxyBase.h"

AWAY_NAMESPACE_BEGIN

class CubeTextureBase : public TextureProxyBase
{
public:
	CubeTextureBase();

	int getSize() { return m_width; }
};

AWAY_NAMESPACE_END