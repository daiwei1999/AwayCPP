#pragma once

#include "Common.h"
#include "MaterialPassBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * OutlinePass is a pass that offsets a mesh and draws it in a single color. This is a pass provided by OutlineMethod.
 */
class OutlinePass : public MaterialPassBase
{
public:
	OutlinePass(unsigned int color = 0, float thickness = 1, bool showInnerLines = true, bool dedicatedMeshes = false);

	unsigned int getColor() { return m_color; }
	void setColor(unsigned int value);
	float getThickness() { return m_offsetData[0]; }
	void setThickness(float value) { m_offsetData[0] = value; }
	bool getShowInnerLines() { return m_showInnerLines; }
	void setShowInnerLines(bool value) { m_showInnerLines = value; }

	void getVertexCode(Shader& vertexShader) override;
	void getFragmentCode(Shader& fragmentShader) override;
	void activate(IContext* context, Camera3D* camera) override;
	void deactivate(IContext* context) override;
	void render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection) override;

private:
	unsigned int m_color;
	float m_colorData[4];
	float m_offsetData[4];
	bool m_showInnerLines;
	bool m_dedicatedMeshes;
};

AWAY_NAMESPACE_END