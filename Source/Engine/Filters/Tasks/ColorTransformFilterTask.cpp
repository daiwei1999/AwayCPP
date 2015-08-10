#include "ColorTransformFilterTask.h"
#include "IContext.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

ColorTransformFilterTask::ColorTransformFilterTask(ColorTransform& colorTransform)
{
	m_colorTransform = colorTransform;
}

void ColorTransformFilterTask::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	float inv = 1 / 255.f;
	m_data[0] = m_colorTransform.m_redMultiplier;
	m_data[1] = m_colorTransform.m_greenMultiplier;
	m_data[2] = m_colorTransform.m_blueMultiplier;
	m_data[3] = m_colorTransform.m_alphaMultiplier;
	m_data[4] = m_colorTransform.m_redOffset * inv;
	m_data[5] = m_colorTransform.m_greenOffset * inv;
	m_data[6] = m_colorTransform.m_blueOffset * inv;
	m_data[7] = m_colorTransform.m_alphaOffset * inv;
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_data, 2);
}

void ColorTransformFilterTask::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.tex(Regs::t0, Regs::v0, Regs::s0);
	fragmentShader.mul(Regs::t0, Regs::t0, Regs::c0);
	fragmentShader.add(Regs::t0, Regs::t0, Regs::c1);
	fragmentShader.mov(Regs::oc, Regs::t0);
}