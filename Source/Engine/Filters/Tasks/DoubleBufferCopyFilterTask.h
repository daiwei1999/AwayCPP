#pragma once

#include "Common.h"
#include "FilterTaskBase.h"

AWAY_NAMESPACE_BEGIN

class DoubleBufferCopyFilterTask : public FilterTaskBase
{
public:
	DoubleBufferCopyFilterTask();

	Texture* getSecondaryInputTexture() { return m_secondaryInputTexture; }
	void activate(IContext* context, Camera3D* camera, Texture* depthTexture) override;

protected:
	void getFragmentCode(Shader& fragmentShader) override;
	void updateTextures(IContext* context) override;

private:
	Texture* m_secondaryInputTexture;
};

AWAY_NAMESPACE_END