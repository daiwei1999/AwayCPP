#include "TextureProjector.h"
#include "PerspectiveLens.h"
#include "OrthographicLens.h"
#include "Texture2DBase.h"
#include "LensEvent.h"

USING_AWAY_NAMESPACE

TextureProjector::TextureProjector(Texture2DBase* texture, bool usePerspectiveLens)
{
	if (usePerspectiveLens)
		m_lens = new PerspectiveLens();
	else
		m_lens = new OrthographicLens();

	m_lens->addEventListener(LensEvent::MATRIX_CHANGED, CALLBACK_OBJECT(onInvalidateLensMatrix));
	m_lens->setAspectRatio((float)texture->getWidth() / texture->getHeight());
	m_texture = texture;
	setRotationX(-90);
}

float TextureProjector::getAspectRatio()
{
	return m_lens->getAspectRatio();
}

void TextureProjector::setAspectRatio(float value)
{
	m_lens->setAspectRatio(value);
}

void TextureProjector::setTexture(Texture2DBase* value)
{
	if (value != m_texture)
		m_texture = value;
}

Matrix3D& TextureProjector::getViewProjection()
{
	if (m_viewProjectionInvalid)
	{
		m_viewProjection.copyFrom(getInverseSceneTransform());
		m_viewProjection.append(m_lens->getMatrix());
		m_viewProjectionInvalid = false;
	}
	return m_viewProjection;
}

void TextureProjector::invalidateSceneTransform()
{
	ObjectContainer3D::invalidateSceneTransform();
	m_viewProjectionInvalid = true;
}

void TextureProjector::onInvalidateLensMatrix(Event* event)
{
	m_viewProjectionInvalid = true;
}