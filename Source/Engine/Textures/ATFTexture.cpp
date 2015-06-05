#include "ATFTexture.h"
#include "ByteArray.h"
#include "IContext.h"
#include "Texture.h"

USING_AWAY_NAMESPACE

ATFTexture::ATFTexture(ByteArray* byteArray)
{
	byteArray->setPosition(12);
	unsigned char tdata = byteArray->readUnsignedByte();
	assert(tdata >> 7 == 0); // normal texture

	switch (tdata & 0x7f)
	{
	case 3:
		m_format = TextureFormat::COMPRESSED;
		break;
	case 5:
		m_format = TextureFormat::COMPRESSED_ALPHA;
		break;
	default:
		assert(false); // not support yet
		break;
	}

	m_width = (int)std::pow(2.f, byteArray->readUnsignedByte());
	m_height = (int)std::pow(2.f, byteArray->readUnsignedByte());
	m_hasMipmaps = byteArray->readUnsignedByte() > 1;
	m_data = byteArray;
}

TextureBase* ATFTexture::createTexture(IContext* context)
{
	return context->createTexture(m_width, m_height, m_format, false);
}

void ATFTexture::uploadContent(TextureBase* texture)
{
	static_cast<Texture*>(texture)->uploadCompressedTextureFromByteArray(m_data, 0);
}