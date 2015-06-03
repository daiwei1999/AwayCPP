#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

enum class TextureType
{
	TWO_D, // 2-dimensional texture
	Cube   // cube texture
};

class TextureBase
{
public:
	TextureBase(TextureType type, int width, int height, TextureFormat format, bool optimizeForRenderToTexture);
	~TextureBase();

	TextureType getType() { return m_type; }
	int getWidth() { return m_width; }
	int getHeight() { return m_height; }
	TextureFormat getFormat() { return m_format; }
	unsigned int getId() { return m_texture; }
#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	unsigned int getAlphaId() { return m_textureAlpha; }
#endif
	unsigned int getFrameBuffer() { return m_frameBuffer; }

protected:
	TextureType m_type;
	int m_width;
	int m_height;
	TextureFormat m_format;
	unsigned int m_texture;
#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	unsigned int m_textureAlpha;
#endif
	unsigned int m_frameBuffer;
};

class Texture : public TextureBase
{
public:
	Texture(int width, int height, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels);

	void uploadFromBitmapData(BitmapData* source, unsigned int miplevel = 0);
	void uploadCompressedTextureFromByteArray(ByteArray* data, unsigned int byteArrayOffset);
};

class CubeTexture : public TextureBase
{
public:
	CubeTexture(int size, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels);

	void uploadFromBitmapData(BitmapData* source, unsigned int side, unsigned int miplevel = 0);
	void uploadCompressedTextureFromByteArray(ByteArray* data, unsigned int byteArrayOffset);
};

AWAY_NAMESPACE_END