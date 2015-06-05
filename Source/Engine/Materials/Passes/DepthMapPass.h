#pragma once

#include "Common.h"
#include "MaterialPassBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * DepthMapPass is a pass that writes depth values to a depth map as a 32-bit value exploded over the 4 texture channels.
 * This is used to render shadow maps, depth maps, etc.
 */
class DepthMapPass : public MaterialPassBase
{
public:
	DepthMapPass();

	float getAlphaThreshold() { return m_alphaThreshold; }
	void setAlphaThreshold(float value);
	Texture2DBase* getAlphaMask() { return m_alphaMask; }
	void setAlphaMask(Texture2DBase* value) { m_alphaMask = value; }

	void getVertexCode(Shader& vertexShader) override;
	void getFragmentCode(Shader& fragmentShader) override;
	void render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection) override;
	void activate(IContext* context, Camera3D* camera) override;

private:
	float m_data[12];
	float m_alphaThreshold;
	Texture2DBase* m_alphaMask;
};

AWAY_NAMESPACE_END