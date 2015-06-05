#pragma once

#include "Common.h"
#include "CubeTextureBase.h"

AWAY_NAMESPACE_BEGIN

class RenderCubeTexture : public CubeTextureBase
{
public:
	RenderCubeTexture(int size);

protected:
	TextureBase* createTexture(IContext* context) override;
	void uploadContent(TextureBase* texture) override;
};

AWAY_NAMESPACE_END