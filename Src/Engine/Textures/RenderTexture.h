#pragma once

#include "Common.h"
#include "Texture2DBase.h"

AWAY_NAMESPACE_BEGIN

class RenderTexture : public Texture2DBase
{
public:
	RenderTexture(int width, int height);

	void setWidth(int value);
	void setHeight(int value);

protected:
	TextureBase* createTexture(IContext* context) override;
	void uploadContent(TextureBase* texture) override;
};

AWAY_NAMESPACE_END