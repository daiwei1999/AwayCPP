#pragma once

#include "Common.h"
#include "Texture2DBase.h"

AWAY_NAMESPACE_BEGIN

class BitmapTexture : public Texture2DBase
{
public:
	BitmapTexture(BitmapData* bitmapData, bool generateMipmaps = true, bool enableDXTEncoding = false);

	BitmapData* getBitmapData() { return m_bitmapData; }
	void setBitmapData(BitmapData* value);

protected:
	TextureBase* createTexture(IContext* context) override;
	void uploadContent(TextureBase* texture) override;

private:
	BitmapData* m_bitmapData;
};

AWAY_NAMESPACE_END