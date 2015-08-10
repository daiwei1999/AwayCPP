#include "FilterBase.h"
#include "FilterTaskBase.h"

USING_AWAY_NAMESPACE

FilterBase::FilterBase()
{
	m_requireDepthRender = false;
}

Texture* FilterBase::getMainInputTexture(IContext* context)
{
	return m_tasks[0]->getMainInputTexture(context);
}

void FilterBase::setTextureWidth(int value)
{
	m_textureWidth = value;
	for (auto task : m_tasks)
		task->setTextureWidth(value);
}

void FilterBase::setTextureHeight(int value)
{
	m_textureHeight = value;
	for (auto task : m_tasks)
		task->setTextureHeight(value);
}

void FilterBase::setRenderTargets(Texture* mainTarget, IContext* context)
{
	m_tasks.back()->setTarget(mainTarget);
}

void FilterBase::update(IContext* context, Camera3D* camera)
{

}

void FilterBase::addTask(FilterTaskBase* task)
{
	m_tasks.push_back(task);
	m_requireDepthRender = m_requireDepthRender || task->requireDepthRender();
}