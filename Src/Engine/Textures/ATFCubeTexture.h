#pragma once

#include "Common.h"
#include "CubeTextureBase.h"

AWAY_NAMESPACE_BEGIN

class ATFCubeTexture : public CubeTextureBase
{
public:
	ATFCubeTexture(ByteArray* byteArray);

protected:
	TextureBase* createTexture(IContext* context) override;
	void uploadContent(TextureBase* texture) override;

private:
	ByteArray* m_data;
};

AWAY_NAMESPACE_END