#pragma once

#include "Common.h"
#include "ObjectContainer3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * TextureProjector is an object in the scene that can be used to project textures onto geometry. To do so,
 * the object's material must have a ProjectiveTextureMethod method added to it with a TextureProjector object
 * passed in the constructor.
 * This can be used for various effects apart from acting like a normal projector, such as projecting fake shadows
 * unto a surface, the impact of light coming through a stained glass window, ...
 */
class TextureProjector : public ObjectContainer3D
{
	DEFINE_CALLBACK_CLASS(TextureProjector, onInvalidateLensMatrix)

public:
	TextureProjector(Texture2DBase* texture, bool usePerspectiveLens = true);

	LensBase* getLens() { return m_lens; }
	float getAspectRatio();
	void setAspectRatio(float value);
	Texture2DBase* getTexture() { return m_texture; }
	void setTexture(Texture2DBase* value);
	Matrix3D& getViewProjection();

protected:
	void invalidateSceneTransform() override;

private:
	void onInvalidateLensMatrix(Event* event);

private:
	LensBase* m_lens;
	Matrix3D m_viewProjection;
	bool m_viewProjectionInvalid;
	Texture2DBase* m_texture;
};

AWAY_NAMESPACE_END