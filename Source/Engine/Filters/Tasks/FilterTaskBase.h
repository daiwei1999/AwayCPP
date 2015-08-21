#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class FilterTaskBase
{
public:
	FilterTaskBase(bool requireDepthRender = false);

	bool requireDepthRender() { return m_requireDepthRender; }
	int getTextureScale() { return m_textureScale; }
	void setTextureScale(int value);
	Texture* getTarget() { return m_target; }
	void setTarget(Texture* value) { m_target = value; }
	int getTextureWidth() { return m_textureWidth; }
	void setTextureWidth(int value);
	int getTextureHeight() { return m_textureHeight; }
	void setTextureHeight(int value);
	Texture* getMainInputTexture(IContext* context);
	Program* getProgram(IContext* context);
	virtual void activate(IContext* context, Camera3D* camera, Texture* depthTexture);
	virtual void deactivate(IContext* context);

protected:
	void invalidateProgram() { m_programInvalid = true; }
	virtual void getVertexCode(Shader& vertexShader);
	virtual void getFragmentCode(Shader& fragmentShader) = 0;
	virtual void updateTextures(IContext* context);

protected:
	Texture* m_mainInputTexture;
	IContext* m_mainInputTextureContext;
	int m_scaledTextureWidth;
	int m_scaledTextureHeight;
	int m_textureWidth;
	int m_textureHeight;
	int m_textureScale;

private:
	bool m_requireDepthRender;
	bool m_textureDimensionsInvalid;
	bool m_programInvalid;
	Program* m_program;
	IContext* m_programContext;
	Texture* m_target;
};

AWAY_NAMESPACE_END