#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class FilterRenderer
{
	DEFINE_CALLBACK_CLASS(FilterRenderer, onRTTResize)

public:
	FilterRenderer(IContext* context);

	bool requireDepthRender() { return m_requireDepthRender; }
	Texture* getMainInputTexture(IContext* context);
	FilterVector& getFilters() { return m_filters; }
	void setFilters(FilterVector& value);
	void render(IContext* context, Camera3D* camera, Texture* depthTexture);

private:
	void updateFilterTasks(IContext* context);
	void updateFilterSizes();
	void onRTTResize(Event* event);

private:
	FilterVector m_filters;
	std::vector<FilterTaskBase*> m_tasks;
	bool m_filterTasksInvalid;
	bool m_requireDepthRender;
	bool m_filterSizesInvalid;
	Texture* m_mainInputTexture;
	RTTBufferManager* m_rttManager;
	IContext* m_context;
};

AWAY_NAMESPACE_END