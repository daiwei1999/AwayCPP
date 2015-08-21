#include "DoubleBufferCopyFilterTask.h"
#include "Shader.h"
#include "Regs.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

DoubleBufferCopyFilterTask::DoubleBufferCopyFilterTask()
{
	m_secondaryInputTexture = nullptr;
}

void DoubleBufferCopyFilterTask::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	Texture* temp = m_secondaryInputTexture;
	m_secondaryInputTexture = m_mainInputTexture;
	m_mainInputTexture = temp;
}

void DoubleBufferCopyFilterTask::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.tex(Regs::oc, Regs::v0, Regs::s0);
}

void DoubleBufferCopyFilterTask::updateTextures(IContext* context)
{
	FilterTaskBase::updateTextures(context);

	m_secondaryInputTexture = context->createTexture(m_textureWidth >> m_textureScale, m_textureHeight >> m_textureScale, TextureFormat::BGRA, true);
}