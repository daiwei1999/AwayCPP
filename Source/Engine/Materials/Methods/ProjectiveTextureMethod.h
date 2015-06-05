#pragma once

#include "Common.h"
#include "EffectMethodBase.h"

AWAY_NAMESPACE_BEGIN

enum class ProjectiveMode
{
	ADD,
	MIX,
	MULTIPLY
};

/**
 * ProjectiveTextureMethod is a material method used to project a texture unto the surface of an object.
 * This can be used for various effects apart from acting like a normal projector, such as projecting fake shadows
 * unto a surface, the impact of light coming through a stained glass window, ...
 */
class ProjectiveTextureMethod : public EffectMethodBase
{
public:
	ProjectiveTextureMethod(TextureProjector* projector, ProjectiveMode mode = ProjectiveMode::MULTIPLY, bool smooth = true);

	TextureProjector* getProjector() { return m_projector; }
	void setProjector(TextureProjector* value) { m_projector = value; }
	ProjectiveMode getMode() { return m_mode; }
	void setMode(ProjectiveMode value);
	bool getSmooth() { return m_smooth; }
	void setSmooth(bool value) { m_smooth = value; }

	void initConstants(MethodVO* vo) override;
	void activate(MethodVO* vo, IContext* context) override;
	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;

private:
	TextureProjector* m_projector;
	ProjectiveMode m_mode;
	bool m_smooth;
	unsigned int m_uvVarying;
};

AWAY_NAMESPACE_END