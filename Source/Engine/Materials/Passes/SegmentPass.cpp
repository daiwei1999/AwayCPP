#include "SegmentPass.h"
#include "Shader.h"
#include "Regs.h"
#include "Matrix3D.h"
#include "IRenderable.h"
#include "IContext.h"
#include "SegmentSet.h"

USING_AWAY_NAMESPACE

SegmentPass::SegmentPass()
{
	
}

void SegmentPass::getVertexCode(Shader& vertexShader)
{
	vertexShader.m44(Regs::op, Regs::a0, Regs::c0);
	vertexShader.mov(Regs::v0, Regs::a1);
}

void SegmentPass::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.mov(Regs::oc, Regs::v0);
}

void SegmentPass::deactivate(IContext* context)
{
	context->setVertexBufferAt(0, nullptr);
	context->setVertexBufferAt(1, nullptr);
}

void SegmentPass::render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	if (static_cast<SegmentSet*>(renderable)->hasData())
	{
		Matrix3D matrix(renderable->getRenderSceneTransform(camera));
		matrix.append(viewProjection);
		context->setProgramConstantsFromMatrix(ProgramType::VERTEX, 0, matrix, true);
		renderable->activateVertexBuffer(0, context);
		context->drawLines(renderable->getIndexBuffer(context), 0, renderable->getTriangleCount());
	}
}