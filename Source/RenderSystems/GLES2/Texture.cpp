#include "Texture.h"
#include "IContext.h"
#include "BitmapData.h"
#include "ByteArray.h"

#if AWAY_PLATFORM == AWAY_PLATFORM_WINDOWS
# if defined(AWAY_PLUGIN_ANGLE)
#  include "GLES2/gl2.h"
# else
#  include "glew.h"
# endif
#elif AWAY_PLATFORM == AWAY_PLATFORM_OSX
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_LINUX
# include <GL/gl.h>
# include <GL/glu.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_IOS
# include <OpenGLES/ES2/gl.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
# include <GLES2/gl2.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_HTML5
# include <GLES2/gl2.h>
# include <EGL/egl.h>
#endif

#ifndef GL_ETC1_RGB8_OES
#define GL_ETC1_RGB8_OES 0x8d64
#endif
#ifndef COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8c00
#endif
#ifndef COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#define COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8c01
#endif
#ifndef COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8c02
#endif
#ifndef COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#define COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8c03
#endif

USING_AWAY_NAMESPACE

TextureBase::TextureBase(TextureType type, int width, int height, TextureFormat format, bool optimizeForRenderToTexture)
{
	m_type = type;
	m_width = width;
	m_height = height;
	m_format = format;
	glGenTextures(1, &m_texture);

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	glGenTextures(1, &m_textureAlpha);
#endif

	if (optimizeForRenderToTexture)
	{
		glGenFramebuffers(1, &m_frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

		if (type == TextureType::TWO_D)
		{
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
		}
		else
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
			for (int i = 0; i < 6; i++)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

			for (int i = 0; i < 6; i++)
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_texture, 0);
		}

		unsigned int renderBuffer;
		glGenRenderbuffers(1, &renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
#ifdef GL_ES_VERSION_2_0
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height);
#else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
#endif
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			assert(false);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

TextureBase::~TextureBase()
{
	glDeleteTextures(1, &m_texture);

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	glDeleteTextures(1, &m_textureAlpha);
#endif
}

Texture::Texture(int width, int height, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels) : TextureBase(TextureType::TWO_D, width, height, format, optimizeForRenderToTexture)
{

}

void Texture::uploadFromBitmapData(BitmapData* source, unsigned int miplevel)
{
	GLenum type, format = source->isTransparent() ? GL_RGBA : GL_RGB;
	if (m_format == TextureFormat::BGRA_PACKED)
		type = GL_UNSIGNED_SHORT_4_4_4_4;
	else if (m_format == TextureFormat::BGR_PACKED)
		type = GL_UNSIGNED_SHORT_5_6_5;
	else
		type = GL_UNSIGNED_BYTE;

	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, miplevel, format, m_width, m_height, 0, format, type, source->getImageData());
}

void Texture::uploadCompressedTextureFromByteArray(ByteArray* data, unsigned int byteArrayOffset)
{
	// parse ATF and load compressed image data(DXT1/DXT5/ETC1/PVRTC4bpp)
	// DXT1->GL_COMPRESSED_RGB_S3TC_DXT1_EXT, DXT5->GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	// ETC1->GL_ETC1_RGB8_OES
	// PVRTC4bpp->COMPRESSED_RGB_PVRTC_4BPPV1_IMG/COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
	data->setEndian(Endian::BIG);
	assert((*data)[7] == 2); // only support ATF version 2.0
	assert((*data)[12] == 3 || (*data)[12] == 5); // only support 'RAW Compressed' or 'RAW Compressed With Alpha'
	data->setPosition(15);
	unsigned char numTextures = data->readUnsignedByte();
	int imageSize, width = m_width, height = m_height;

#if AWAY_PLATFORM == AWAY_PLATFORM_IOS
	GLenum internalformat = (m_format == TF_COMPRESSED) ? COMPRESSED_RGB_PVRTC_4BPPV1_IMG : COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#elif AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	GLenum internalformat = GL_ETC1_RGB8_OES;
#else
	GLenum internalformat = (m_format == TextureFormat::COMPRESSED) ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif

	glBindTexture(GL_TEXTURE_2D, m_texture);
	for (unsigned char i = 0; i < numTextures; i++)
	{
		// DXT1
		imageSize = data->readUnsignedInt();
#if AWAY_PLATFORM != AWAY_PLATFORM_IOS && AWAY_PLATFORM != AWAY_PLATFORM_ANDROID
		glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, imageSize, data->getByteData());
#endif
		data->setPosition(data->getPosition() + imageSize);

		// PVRTC
		imageSize = data->readUnsignedInt();
#if AWAY_PLATFORM == AWAY_PLATFORM_IOS
		glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, imageSize, data->getByteData());
#endif
		data->setPosition(data->getPosition() + imageSize);

		// ETC1
		imageSize = data->readUnsignedInt();
#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
		if (m_format == TextureFormat::COMPRESSED_ALPHA)
			glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, imageSize >> 1, data->getByteData());
		else
			glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, imageSize, data->getByteData());
#endif
		data->setPosition(data->getPosition() + imageSize);

		// reduce width by half
		if (width > 1)
			width >>= 1;

		// reduce height by half
		if (height > 1)
			height >>= 1;
	}

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	if (m_format == TextureFormat::COMPRESSED_ALPHA)
	{
		width = m_width;
		height = m_height;
		data->setPosition(16);

		glBindTexture(GL_TEXTURE_2D, m_textureAlpha);
		for (unsigned char i = 0; i < numTextures; i++)
		{
			// skip DXT1
			imageSize = data->readUnsignedInt();
			data->setPosition(data->getPosition() + imageSize);

			// skip PVRTC
			imageSize = data->readUnsignedInt();
			data->setPosition(data->getPosition() + imageSize);

			// load ETC1(alpha part)
			imageSize = data->readUnsignedInt();
			glCompressedTexImage2D(GL_TEXTURE_2D, i, internalformat, width, height, 0, imageSize >> 1, data->getByteData() + (imageSize >> 1));
			data->setPosition(data->getPosition() + imageSize);

			// reduce width by half
			if (width > 1)
				width >>= 1;

			// reduce height by half
			if (height > 1)
				height >>= 1;
		}
	}
#endif
}

CubeTexture::CubeTexture(int size, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels) : TextureBase(TextureType::Cube, size, size, format, optimizeForRenderToTexture)
{

}

void CubeTexture::uploadFromBitmapData(BitmapData* source, unsigned int side, unsigned int miplevel)
{
	GLenum type, format = source->isTransparent() ? GL_RGBA : GL_RGB;
	if (m_format == TextureFormat::BGRA_PACKED)
		type = GL_UNSIGNED_SHORT_4_4_4_4;
	else if (m_format == TextureFormat::BGR_PACKED)
		type = GL_UNSIGNED_SHORT_5_6_5;
	else
		type = GL_UNSIGNED_BYTE;

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, miplevel, format, m_width, m_height, 0, format, type, source->getImageData());
}

void CubeTexture::uploadCompressedTextureFromByteArray(ByteArray* data, unsigned int byteArrayOffset)
{
	// parse ATF and load compressed image data(DXT1/DXT5/ETC1/PVRTC4bpp)
	// DXT1->GL_COMPRESSED_RGB_S3TC_DXT1_EXT, DXT5->GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
	// ETC1->GL_ETC1_RGB8_OES
	// PVRTC4bpp->COMPRESSED_RGB_PVRTC_4BPPV1_IMG/COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
	data->setEndian(Endian::BIG);
	assert((*data)[7] == 2); // only support ATF version 2.0
	assert((*data)[12] == 0x83 || (*data)[12] == 0x85); // only support 'RAW Compressed' or 'RAW Compressed With Alpha'
	data->setPosition(15);
	unsigned char numTextures = data->readUnsignedByte();
	int imageSize, width, height;

#if AWAY_PLATFORM == AWAY_PLATFORM_IOS
	GLenum internalformat = (m_format == TF_COMPRESSED) ? COMPRESSED_RGB_PVRTC_4BPPV1_IMG : COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#elif AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	GLenum internalformat = GL_ETC1_RGB8_OES;
#else
	GLenum internalformat = (m_format == TextureFormat::COMPRESSED) ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
#endif

	GLenum target;
	GLenum targets[6] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z };
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	for (int j = 0; j < 6; j++)
	{
		target = targets[j];
		width = m_width;
		height = m_height;
		for (unsigned char i = 0; i < numTextures; i++)
		{
			// DXT1
			imageSize = data->readUnsignedInt();
#if AWAY_PLATFORM != AWAY_PLATFORM_IOS && AWAY_PLATFORM != AWAY_PLATFORM_ANDROID
			glCompressedTexImage2D(target, i, internalformat, width, height, 0, imageSize, data->getByteData());
#endif
			data->setPosition(data->getPosition() + imageSize);

			// PVRTC
			imageSize = data->readUnsignedInt();
#if AWAY_PLATFORM == AWAY_PLATFORM_IOS
			glCompressedTexImage2D(target, i, internalformat, width, height, 0, imageSize, data->getByteData());
#endif
			data->setPosition(data->getPosition() + imageSize);

			// ETC1
			imageSize = data->readUnsignedInt();
#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
			if (m_format == TextureFormat::COMPRESSED_ALPHA)
				glCompressedTexImage2D(target, i, internalformat, width, height, 0, imageSize >> 1, data->getByteData());
			else
				glCompressedTexImage2D(target, i, internalformat, width, height, 0, imageSize, data->getByteData());
#endif
			data->setPosition(data->getPosition() + imageSize);

			// reduce width by half
			if (width > 1)
				width >>= 1;

			// reduce height by half
			if (height > 1)
				height >>= 1;
		}
	}

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
	if (m_format == TextureFormat::COMPRESSED_ALPHA)
	{
		data->setPosition(16);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureAlpha);
		for (int j = 0; j < 6; j++)
		{
			target = targets[j];
			width = m_width;
			height = m_height;
			for (unsigned char i = 0; i < numTextures; i++)
			{
				// skip DXT1
				imageSize = data->readUnsignedInt();
				data->setPosition(data->getPosition() + imageSize);

				// skip PVRTC
				imageSize = data->readUnsignedInt();
				data->setPosition(data->getPosition() + imageSize);

				// load ETC1(alpha part)
				imageSize = data->readUnsignedInt();
				glCompressedTexImage2D(target, i, internalformat, width, height, 0, imageSize >> 1, data->getByteData() + (imageSize >> 1));
				data->setPosition(data->getPosition() + imageSize);

				// reduce width by half
				if (width > 1)
					width >>= 1;

				// reduce height by half
				if (height > 1)
					height >>= 1;
			}
		}
	}
#endif
}