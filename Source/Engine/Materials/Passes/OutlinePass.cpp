#include "OutlinePass.h"
#include "Shader.h"
#include "Regs.h"
#include "IContext.h"
#include "Matrix3D.h"
#include "IRenderable.h"

USING_AWAY_NAMESPACE

OutlinePass::OutlinePass(unsigned int color, float thickness, bool showInnerLines, bool dedicatedMeshes)
{
	setColor(color);
	m_offsetData[0] = thickness;

	m_numUsedStreams = 2;
	m_numUsedVertexConstants = 5;
	m_showInnerLines = showInnerLines;
	m_dedicatedMeshes = dedicatedMeshes;

	m_animatableAttributes = { Regs::a0, Regs::a1 };
	m_animationTargetRegisters = { Regs::t0, Regs::t1 };
}

void OutlinePass::setColor(unsigned int value)
{
	m_color = value;
	m_colorData[0] = ((value >> 16) & 0xff) / 255.f;
	m_colorData[1] = ((value >> 8) & 0xff) / 255.f;
	m_colorData[2] = (value & 0xff) / 255.f;
}

void OutlinePass::getVertexCode(Shader& vertexShader)
{
	vertexShader.mul(Regs::t7, Regs::t1, Regs::c4 ^ Regs::x);
	vertexShader.add(Regs::t7, Regs::t7, Regs::t0);
	vertexShader.mov(Regs::t7 ^ Regs::w, Regs::t0 ^ Regs::w);
	vertexShader.m44(Regs::op, Regs::t7, Regs::c0);
}

void OutlinePass::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.mov(Regs::oc, Regs::c0);
}

void OutlinePass::activate(IContext* context, Camera3D* camera)
{
	MaterialPassBase::activate(context, camera);

	// do not write depth if not drawing inner lines (will cause the overdraw to hide inner lines)
	if (!m_showInnerLines)
		context->setDepthTest(false, CompareMode::LESS);

	context->setCulling(TriangleFace::FRONT);
	context->setProgramConstantsFromVector(ProgramType::VERTEX, 4, m_offsetData, 1);
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_colorData, 1);
}

void OutlinePass::deactivate(IContext* context)
{
	MaterialPassBase::deactivate(context);

	if (!m_showInnerLines)
		context->setDepthTest(true, CompareMode::LESS);
}

void OutlinePass::render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	Matrix3D matrix(renderable->getRenderSceneTransform(camera));
	matrix.append(viewProjection);

	context->setProgramConstantsFromMatrix(ProgramType::VERTEX, 0, matrix, true);
	renderable->activateVertexBuffer(0, context);
	renderable->activateVertexNormalBuffer(1, context);
	context->drawTriangles(renderable->getIndexBuffer(context), 0, renderable->getTriangleCount());
}