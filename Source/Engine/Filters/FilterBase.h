#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class FilterBase
{
public:
	FilterBase();

	bool requireDepthRender() { return m_requireDepthRender; }
	const std::vector<FilterTaskBase*>& getTasks() { return m_tasks; }
	Texture* getMainInputTexture(IContext* context);
	int getTextureWidth() { return m_textureWidth; }
	void setTextureWidth(int value);
	int getTextureHeight() { return m_textureHeight; }
	void setTextureHeight(int value);
	void setRenderTargets(Texture* mainTarget, IContext* context);
	virtual void update(IContext* context, Camera3D* camera);

protected:
	void addTask(FilterTaskBase* task);

private:
	std::vector<FilterTaskBase*> m_tasks;
	bool m_requireDepthRender;
	int m_textureWidth;
	int m_textureHeight;
};

AWAY_NAMESPACE_END