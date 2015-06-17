#pragma once

#include "Common.h"
#include "EventDispatcher.h"
#include "Rectangle.h"

AWAY_NAMESPACE_BEGIN

/**
 * MaterialPassBase provides an abstract base class for material shader passes. A material pass constitutes at least
 * a render call per required renderable.
 */
class MaterialPassBase : public EventDispatcher
{
	DEFINE_CALLBACK_CLASS(MaterialPassBase, onLightsChange)

public:
	MaterialPassBase(bool renderToTexture = false);
	virtual ~MaterialPassBase();

	MaterialBase* getMaterial() { return m_material; }
	void setMaterial(MaterialBase* value) { m_material = value; }
	bool getMipmap() { return m_mipmap; }
	void setMipmap(bool value);
	bool getSmooth() { return m_smooth; }
	void setSmooth(bool value);
	bool getRepeat() { return m_repeat; }
	void setRepeat(bool value);
	bool getBothSides() { return m_bothSides; }
	void setBothSides(bool value) { m_bothSides = value; }
	bool getAlphaPremultiplied() { return m_alphaPremultiplied; }
	void setAlphaPremultiplied(bool value);
	CompareMode getDepthCompareMode() { return m_depthCompareMode; }
	void setDepthCompareMode(CompareMode value) { m_depthCompareMode = value; }
	IAnimationSet* getAnimationSet() { return m_animationSet; }
	void setAnimationSet(IAnimationSet* value);
	LightPickerBase* getLightPicker() { return m_lightPicker; }
	void setLightPicker(LightPickerBase* value);

	bool renderToTexture() { return m_renderToTexture; }
	int getUsedStreamCount() { return m_numUsedStreams; }
	int getUsedVertexConstantCount() { return m_numUsedVertexConstants; }
	int getUsedVaryingCount() { return m_numUsedVaryings; }
	int getUsedFragmentConstantCount() { return m_numUsedFragmentConstants; }
	bool needFragmentAnimation() { return m_needFragmentAnimation; }
	bool needUVAnimation() { return m_needUVAnimation; }

	void updateAnimationState(IRenderable* renderable, IContext* context, Camera3D* camera);
	virtual void render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection) = 0;
	virtual void getVertexCode(Shader& vertexShader) = 0;
	virtual void getFragmentCode(Shader& fragmentShader) = 0;
	void setBlendMode(BlendMode value);
	virtual void activate(IContext* context, Camera3D* camera);
	virtual void deactivate(IContext* context);
	virtual void invalidateShaderProgram(bool updateMaterial = true);
	virtual void updateProgram(IContext* context);

protected:
	virtual void updateLights();

private:
	void onLightsChange(Event* event);

public:
	int m_programId;
	Program* m_program;
	AnimationRegisterCache* m_animationRegisterCache;

protected:
	MaterialBase* m_material;
	IAnimationSet* m_animationSet;
	int m_numUsedStreams;
	int m_numUsedTextures;
	int m_numUsedVertexConstants;
	int m_numUsedFragmentConstants;
	int m_numUsedVaryings;
	bool m_smooth;
	bool m_repeat;
	bool m_mipmap;
	bool m_enableBlending;
	CompareMode m_depthCompareMode;
	BlendFactor m_blendFactorSource;
	BlendFactor m_blendFactorDest;
	LightPickerBase* m_lightPicker;
	std::vector<unsigned int> m_animatableAttributes;
	std::vector<unsigned int> m_animationTargetRegisters;
	unsigned int m_shadedTarget;
	bool m_alphaPremultiplied;
	bool m_needFragmentAnimation;
	bool m_needUVAnimation;
	unsigned int m_uvTarget, m_uvSource;

private:
	IContext* m_context;
	bool m_bothSides;
	bool m_renderToTexture;
	bool m_oldDepthStencil;
	TextureBase* m_oldTarget;
	int m_oldSurface;
	Rectangle m_oldRect;

	static int m_previousUsedStreams;
	static int m_previousUsedTexs;
};

AWAY_NAMESPACE_END