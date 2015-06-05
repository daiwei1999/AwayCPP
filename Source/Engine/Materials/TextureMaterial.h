#pragma once

#include "Common.h"
#include "SinglePassMaterialBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * TextureMaterial is a single-pass material that uses a texture to define the surface's diffuse reflection colour (albedo).
 */
class TextureMaterial : public SinglePassMaterialBase
{
public:
	TextureMaterial(Texture2DBase* texture = nullptr, bool smooth = true, bool repeat = false, bool mipmap = true);

	bool getAnimateUVs();
	void setAnimateUVs(bool value);
	float getAlpha();
	void setAlpha(float value);
	Texture2DBase* getTexture();
	void setTexture(Texture2DBase* value);
	Texture2DBase* getAmbientTexture();
	void setAmbientTexture(Texture2DBase* value);
};

AWAY_NAMESPACE_END