#pragma once

#include "Common.h"
#include "Matrix3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * RendererBase forms an abstract base class for classes that are used in the rendering pipeline to render geometry
 * to the back buffer or a texture.
 */
class RendererBase
{
public:
	RendererBase();

	IContext* getContext() { return m_context; }
	virtual void setContext(IContext* value);
	IEntitySorter* getRenderableSorter() { return m_renderableSorter; }
	void setRenderableSorter(IEntitySorter* value) { m_renderableSorter = value; }
	bool getClearOnRender() { return m_clearOnRender; }
	void setClearOnRender(bool value) { m_clearOnRender = value; }
	float getBackgroundR() { return m_backgroundR; }
	virtual void setBackgroundR(float value) { m_backgroundR = value; }
	float getBackgroundG() { return m_backgroundG; }
	virtual void setBackgroundG(float value) { m_backgroundG = value; }
	float getBackgroundB() { return m_backgroundB; }
	virtual void setBackgroundB(float value) { m_backgroundB = value; }
	float getBackgroundA() { return m_backgroundA; }
	void setBackgroundA(float value) { m_backgroundA = value; }
	Texture2DBase* getBackground() { return m_background; }
	void setBackground(Texture2DBase* value);
	bool getShareContext() { return m_shareContext; }
	void setShareContext(bool value) { m_shareContext = value; }
	int getAntiAlias() { return m_antiAlias; }
	void setAntiAlias(int value) { m_antiAlias = value; }
	float getTextureRatioX() { return m_textureRatioX; }
	void setTextureRatioX(float value) { m_textureRatioX = value; }
	float getTextureRatioY() { return m_textureRatioY; }
	void setTextureRatioY(float value) { m_textureRatioY = value; }

	EntityCollector* createEntityCollector();
	void render(EntityCollector* entityCollector, TextureBase* target = nullptr, Rectangle<int>* scissorRect = nullptr, int surfaceSelector = 0);

protected:
	virtual void executeRender(EntityCollector* entityCollector, TextureBase* target = nullptr, Rectangle<int>* scissorRect = nullptr, int surfaceSelector = 0);
	virtual void draw(EntityCollector* entityCollector, TextureBase* target) = 0;

protected:
	IContext* m_context;
	float m_backgroundR, m_backgroundG, m_backgroundB, m_backgroundA;
	
	IEntitySorter* m_renderableSorter;
	Texture2DBase* m_background;
	bool m_shareContext;
	int m_antiAlias;
	float m_textureRatioX, m_textureRatioY;
	Matrix3D m_rttViewProjectionMatrix;

private:
	BackgroundImageRenderer* m_backgroundImageRenderer;
	bool m_clearOnRender;
};

AWAY_NAMESPACE_END