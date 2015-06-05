#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class TextureProxyBase
{
public:
	TextureProxyBase();
	virtual ~TextureProxyBase();

	bool hasMipMaps() { return m_hasMipmaps; }
	TextureFormat getFormat() { return m_format; }
	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	TextureBase* getTextureForContext(IContext* context);
	void invalidateContent();

protected:
	void setSize(int width, int height);
	void invalidateSize();
	virtual TextureBase* createTexture(IContext* context) = 0;
	virtual void uploadContent(TextureBase* texture) = 0;

protected:
	TextureFormat m_format;
	bool m_hasMipmaps;
	TextureBase* m_texture;
	IContext* m_context;
	bool m_textureInvalid;
	int m_width;
	int m_height;
};

AWAY_NAMESPACE_END