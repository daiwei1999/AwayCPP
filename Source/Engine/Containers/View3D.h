#pragma once 

#include "Common.h"
#include "Rectangle.h"

AWAY_NAMESPACE_BEGIN

class View3D
{
	DEFINE_CALLBACK_CLASS(View3D, onScenePartitionChanged)

public:
	View3D(Scene3D* scene = nullptr, Camera3D* camera = nullptr, RendererBase* renderer = nullptr);

	IContext* getContext() { return m_context; }
	void setContext(IContext* value);
	Texture2DBase* getBackground() { return m_background; }
	void setBackground(Texture2DBase* value);
	bool getLayeredView() { return m_layeredView; }
	void setLayeredView(bool value) { m_layeredView = value; }
	RendererBase* getRenderer() { return m_renderer; }
	void setRenderer(RendererBase* value);
	unsigned int getBackgroundColor() { return m_backgroundColor; }
	void setBackgroundColor(unsigned int value);
	float getBackgroundAlpha() { return m_backgroundAlpha; }
	void setBackgroundAlpha(float value);
	Camera3D* getCamera() { return m_camera; }
	void setCamera(Camera3D* value);
	Scene3D* getScene() { return m_scene; }
	void setScene(Scene3D* value);
	float getX() { return m_x; }
	void setX(float value);
	float getY() { return m_y; }
	void setY(float value);
	float getWidth() { return m_width; }
	void setWidth(float value);
	float getHeight() { return m_height; }
	void setHeight(float value);
	bool getVisible() { return m_visible; }
	void setVisible(bool value);
	unsigned int getAntiAlias() { return m_antiAlias; }
	void setAntiAlias(unsigned int value);

	void render();
	void project(Vector3D* point3d, Vector3D* result);
	void unproject(float sX, float sY, float sZ, Vector3D* result);
	void getRay(float sX, float sY, float sZ, Vector3D* result);
	EntityCollector* getEntityCollector() { return m_entityCollector; }

protected:
	void updateBackBuffer();
	void updateGlobalPos();
	void updateViewSizeData();
	void renderDepthPrepass(EntityCollector* entityCollector);
	void renderSceneDepthToTexture(EntityCollector* entityCollector);

private:
	void initDepthTexture(IContext* context);
	void onLensChanged(Event* event);

private:
	void onScenePartitionChanged(Event* event);

protected:
	Scene3D* m_scene;
	Camera3D* m_camera;
	RendererBase* m_renderer;
	EntityCollector* m_entityCollector;
	float m_aspectRatio;
	IContext* m_context;
	bool m_visible;
	bool m_backBufferInvalid;
	Rectangle m_scissorRect;

private:
	float m_x;
	float m_y;
	float m_width;
	float m_height;
	unsigned int m_backgroundColor;
	float m_backgroundAlpha;
	Texture2DBase* m_background;
	unsigned int m_antiAlias;
	bool m_globalPosDirty;
	bool m_layeredView;
	bool m_scissorRectDirty;
	bool m_viewportDirty;
};

AWAY_NAMESPACE_END