#pragma once

#include "Common.h"
#include "FilterTaskBase.h"

AWAY_NAMESPACE_BEGIN

class ColorMatrixFilterTask : public FilterTaskBase
{
public:
	ColorMatrixFilterTask(const float matrix[20]);

	void activate(IContext* context, Camera3D* camera, Texture* depthTexture) override;

protected:
	void getFragmentCode(Shader& fragmentShader) override;

private:
	float m_matrix[20];
	float m_colors[16];
	float m_offset[4];
};

AWAY_NAMESPACE_END