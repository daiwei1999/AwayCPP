#include "OutlineMethod.h"
#include "OutlinePass.h"
#include "MethodVO.h"

USING_AWAY_NAMESPACE

OutlineMethod::OutlineMethod(unsigned int color, float thickness, bool showInnerLines, bool dedicatedWaterProofMesh)
{
	m_outlinePass = new OutlinePass(color, thickness, showInnerLines, dedicatedWaterProofMesh);
	m_passes.push_back(m_outlinePass);
}

unsigned int OutlineMethod::getColor()
{
	return m_outlinePass->getColor();
}

void OutlineMethod::setColor(unsigned int value)
{
	m_outlinePass->setColor(value);
}

float OutlineMethod::getThickness()
{
	return m_outlinePass->getThickness();
}

void OutlineMethod::setThickness(float value)
{
	m_outlinePass->setThickness(value);
}

bool OutlineMethod::getShowInnerLines()
{
	return m_outlinePass->getShowInnerLines();
}

void OutlineMethod::setShowInnerLines(bool value)
{
	m_outlinePass->setShowInnerLines(value);
}

void OutlineMethod::initVO(MethodVO* vo)
{
	vo->m_needsNormals = true;
}

void OutlineMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{

}