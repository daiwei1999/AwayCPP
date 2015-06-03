#include "RenderCubeTexture.h"
#include "TextureUtils.h"
#include "IContext.h"
#include "Texture.h"

USING_AWAY_NAMESPACE

RenderCubeTexture::RenderCubeTexture(int size)
{
	CubeTextureBase::setSize(size, size);
}

TextureBase* RenderCubeTexture::createTexture(IContext* context)
{
	return context->createCubeTexture(m_width, TextureFormat::BGRA, true);
}

void RenderCubeTexture::uploadContent(TextureBase* texture)
{

}