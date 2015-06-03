#pragma once

#include "Common.h"
#include "MaterialPassBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * DistanceMapPass is a pass that writes distance values to a depth map as a 32-bit value exploded over the 4 texture channels.
 * This is used to render omnidirectional shadow maps.
 */
class DistanceMapPass : public MaterialPassBase
{
public:
	DistanceMapPass();

	float getAlphaThreshold() { return m_alphaThreshold; }
	void setAlphaThreshold(float value);
	Texture2DBase* getAlphaMask() { return m_alphaMask; }
	void setAlphaMask(Texture2DBase* value) { m_alphaMask = value; }

	void getVertexCode(Shader& vertexShader) override;
	void getFragmentCode(Shader& fragmentShader) override;
	void render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection) override;
	void activate(IContext* context, Camera3D* camera) override;

private:
	float m_fragmentData[12];
	float m_vertexData[4];
	float m_alphaThreshold;
	Texture2DBase* m_alphaMask;
};

AWAY_NAMESPACE_END