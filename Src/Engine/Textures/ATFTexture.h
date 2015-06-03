#pragma once

#include "Common.h"
#include "Texture2DBase.h"

AWAY_NAMESPACE_BEGIN

class ATFTexture : public Texture2DBase
{
public:
	ATFTexture(ByteArray* byteArray);

protected:
	TextureBase* createTexture(IContext* context) override;
	void uploadContent(TextureBase* texture) override;

private:
	ByteArray* m_data;
};

AWAY_NAMESPACE_END