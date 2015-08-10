#include "ColorMatrixFilterTask.h"
#include "IContext.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

ColorMatrixFilterTask::ColorMatrixFilterTask(const float matrix[20])
{
	std::memcpy(m_matrix, matrix, 20 * sizeof(float));
}

void ColorMatrixFilterTask::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	m_colors[0] = m_matrix[0];
	m_colors[1] = m_matrix[1];
	m_colors[2] = m_matrix[2];
	m_colors[3] = m_matrix[3];
	m_offset[0] = m_matrix[4];
	
	m_colors[4] = m_matrix[5];
	m_colors[5] = m_matrix[6];
	m_colors[6] = m_matrix[7];
	m_colors[7] = m_matrix[8];
	m_offset[1] = m_matrix[9];
	
	m_colors[8] = m_matrix[10];
	m_colors[9] = m_matrix[11];
	m_colors[10] = m_matrix[12];
	m_colors[11] = m_matrix[13];
	m_offset[2] = m_matrix[14];
	
	m_colors[12] = m_matrix[15];
	m_colors[13] = m_matrix[16];
	m_colors[14] = m_matrix[17];
	m_colors[15] = m_matrix[18];
	m_offset[3] = m_matrix[19];

	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_colors, 4);
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 4, m_offset, 1);
}

void ColorMatrixFilterTask::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.tex(Regs::t0, Regs::v0, Regs::s0);
	fragmentShader.m44(Regs::t0, Regs::t0, Regs::c0);
	fragmentShader.add(Regs::t0, Regs::t0, Regs::c4);
	fragmentShader.mov(Regs::oc, Regs::t0);
}