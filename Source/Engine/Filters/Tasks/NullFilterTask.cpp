#include "NullFilterTask.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

NullFilterTask::NullFilterTask()
{

}

void NullFilterTask::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.tex(Regs::oc, Regs::v0, Regs::s0);
}