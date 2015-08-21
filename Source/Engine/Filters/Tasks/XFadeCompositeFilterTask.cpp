#include "XFadeCompositeFilterTask.h"
#include "IContext.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

XFadeCompositeFilterTask::XFadeCompositeFilterTask(float amount)
{
	m_data[0] = amount;
	m_data[1] = m_data[2] = m_data[3] = 0;
}

void XFadeCompositeFilterTask::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_data, 1);
	context->setSamplerStateAt(1, WrapMode::CLAMP, TextureFilter::NEAREST, MipFilter::MIPNONE);
	context->setTextureAt(1, m_overlayTexture);
}

void XFadeCompositeFilterTask::deactivate(IContext* context)
{
	context->setTextureAt(1, nullptr);
}

void XFadeCompositeFilterTask::getFragmentCode(Shader& fragmentShader)
{
	// (color1 - color0) * amount + color0
	fragmentShader.tex(Regs::t0, Regs::v0, Regs::s0);
	fragmentShader.tex(Regs::t1, Regs::v0, Regs::s1);
	fragmentShader.sub(Regs::t1, Regs::t1, Regs::t0);
	fragmentShader.mul(Regs::t1, Regs::t1, Regs::c0 ^ Regs::x);
	fragmentShader.add(Regs::oc, Regs::t1, Regs::t0);
}