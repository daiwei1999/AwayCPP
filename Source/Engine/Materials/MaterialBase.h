#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

enum class BlendMode
{
	ADD,
	ALPHA,
	LAYER,
	MULTIPLY,
	NORMAL,
	SCREEN
};

/**
 * MaterialBase forms an abstract base class for any material.
 * A material consists of several passes, each of which constitutes at least one render call. Several passes could
 * be used for special effects (render lighting for many lights in several passes, render an outline in a separate
 * pass) or to provide additional render-to-texture passes (rendering diffuse light to texture for texture-space
 * subsurface scattering, or rendering a depth map for specialized self-shadowing).
 *
 * Away3D provides default materials trough SinglePassMaterialBase and MultiPassMaterialBase, which use modular
 * methods to build the shader code. MaterialBase can be extended to build specific and high-performance custom
 * shaders, or entire new material frameworks.
 */
class MaterialBase
{
	DEFINE_CALLBACK_CLASS(MaterialBase, onDepthPassChange)
	DEFINE_CALLBACK_CLASS(MaterialBase, onDistancePassChange)
	DEFINE_CALLBACK_CLASS(MaterialBase, onPassChange)

public:
	MaterialBase();
	~MaterialBase();

	LightPickerBase* getLightPicker() { return m_lightPicker; }
	void setLightPicker(LightPickerBase* value);

	bool getMipmap() { return m_mipmap; }
	void setMipmap(bool value);

	bool getSmooth() { return m_smooth; }
	void setSmooth(bool value);

	CompareMode getDepthCompareMode() { return m_depthCompareMode; }
	virtual void setDepthCompareMode(CompareMode value) { m_depthCompareMode = value; }

	bool getRepeat() { return m_repeat; }
	void setRepeat(bool value);

	bool getBothSides() { return m_bothSides; }
	void setBothSides(bool value);

	BlendMode getBlendMode() { return m_blendMode; }
	virtual void setBlendMode(BlendMode value) { m_blendMode = value; }

	bool getAlphaPremultiplied() { return m_alphaPremultiplied; }
	void setAlphaPremultiplied(bool value);

	virtual bool requiresBlending() { return m_blendMode != BlendMode::NORMAL; }
	unsigned int getUniqueId() { return m_uniqueId; }
	unsigned int getRenderOrderId() { return m_renderOrderId; }
	unsigned int getDepthPassId() { return m_depthPassId; }
	int getPassCount() { return m_numPasses; }
	bool hasDepthAlphaThreshold();

	virtual void activateForDepth(IContext* context, Camera3D* camera, bool distanceBased = false);
	void deactivateForDepth(IContext* context);
	void renderDepth(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection);
	bool passRendersToTexture(int index);
	void activatePass(int index, IContext* context, Camera3D* camera);
	void deactivatePass(int index, IContext* context);
	void renderPass(int index, IRenderable* renderable, IContext* context, EntityCollector* entityCollector, Matrix3D& viewProjection);
	void invalidatePasses(MaterialPassBase* triggerPass);

	void addOwner(IMaterialOwner* owner);
	void removeOwner(IMaterialOwner* owner);

	virtual void updateMaterial(IContext* context);
	void deactivate(IContext* context);

protected:
	void addPass(MaterialPassBase* pass);
	void removePass(MaterialPassBase* pass);
	void clearPasses();

private:
	void onPassChange(Event* event);
	void onDepthPassChange(Event* event);
	void onDistancePassChange(Event* event);

protected:
	BlendMode m_blendMode;
	int m_numPasses;
	MaterialPassVector m_passes;
	bool m_mipmap;
	bool m_smooth;
	bool m_repeat;
	DepthMapPass* m_depthPass;
	DistanceMapPass* m_distancePass;
	LightPickerBase* m_lightPicker;

private:
	static unsigned int MATERIAL_ID_COUNT;
	unsigned int m_uniqueId;
	unsigned int m_renderOrderId;
	unsigned int m_depthPassId;
	IAnimationSet* m_animationSet;
	std::vector<IMaterialOwner*> m_owners;

	bool m_bothSides;
	bool m_alphaPremultiplied;
	bool m_distanceBasedDepthRender;
	CompareMode m_depthCompareMode;
};

AWAY_NAMESPACE_END