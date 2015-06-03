#include "TextureProxyBase.h"
#include "IContext.h"
#include "Texture.h"

USING_AWAY_NAMESPACE

TextureProxyBase::TextureProxyBase()
{
	m_format = TextureFormat::BGRA;
	m_hasMipmaps = true;
	m_texture = nullptr;
	m_context = nullptr;
}

TextureProxyBase::~TextureProxyBase()
{

}

TextureBase* TextureProxyBase::getTextureForContext(IContext* context)
{
	if (!m_texture || m_context != context)
	{
		m_texture = createTexture(context);
		m_context = context;
		m_textureInvalid = true;
	}
	if (m_textureInvalid)
	{
		uploadContent(m_texture);
		m_textureInvalid = false;
	}
	return m_texture;
}

void TextureProxyBase::invalidateContent()
{
	m_textureInvalid = true;
}

void TextureProxyBase::setSize(int width, int height)
{
	if (m_width != width || m_height != height)
		invalidateSize();

	m_width = width;
	m_height = height;
}

void TextureProxyBase::invalidateSize()
{
	if (m_texture)
	{
		delete m_texture;
		m_texture = nullptr;
	}
}