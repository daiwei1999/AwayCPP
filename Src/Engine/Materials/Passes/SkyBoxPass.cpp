#include "SkyBoxPass.h"
#include "Shader.h"
#include "Regs.h"
#include "Tex.h"
#include "Camera3D.h"
#include "LensBase.h"
#include "IContext.h"
#include "IRenderable.h"
#include "CubeTextureBase.h"

USING_AWAY_NAMESPACE

SkyBoxPass::SkyBoxPass(CubeTextureBase* cubeMap)
{
	m_cubeMap = cubeMap;
	m_numUsedTextures = 1;
	m_numUsedVertexConstants = 6;
	m_vertexData[0] = m_vertexData[1] = m_vertexData[2] = m_vertexData[3] = 0;
	m_vertexData[4] = m_vertexData[5] = m_vertexData[6] = m_vertexData[7] = 1;
}

void SkyBoxPass::getVertexCode(Shader& vertexShader)
{
	vertexShader.mul(Regs::t0, Regs::a0, Regs::c5);
	vertexShader.add(Regs::t0, Regs::t0, Regs::c4);
	vertexShader.m44(Regs::op, Regs::t0, Regs::c0);
	vertexShader.mov(Regs::v0, Regs::a0);
}

void SkyBoxPass::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.tex(Regs::t0, Regs::v0, Regs::s0 | Tex::CUBE);
	fragmentShader.mov(Regs::oc, Regs::t0);
}

void SkyBoxPass::render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	Vector3D* cameraPos = camera->getScenePosition();
	m_vertexData[0] = cameraPos->m_x;
	m_vertexData[1] = cameraPos->m_y;
	m_vertexData[2] = cameraPos->m_z;
	m_vertexData[4] = m_vertexData[5] = m_vertexData[6] = camera->getLens()->getFar() / std::sqrt(3.f);

	context->setProgramConstantsFromMatrix(ProgramType::VERTEX, 0, viewProjection, true);
	context->setProgramConstantsFromVector(ProgramType::VERTEX, 4, m_vertexData, 2);
	renderable->activateVertexBuffer(0, context);
	context->drawTriangles(renderable->getIndexBuffer(context), 0, renderable->getTriangleCount());
}

void SkyBoxPass::activate(IContext* context, Camera3D* camera)
{
	MaterialPassBase::activate(context, camera);

	context->setDepthTest(false, CompareMode::LESS);
	context->setSamplerStateAt(0, WrapMode::CLAMP, TextureFilter::LINEAR, m_cubeMap->hasMipMaps() ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
	context->setTextureAt(0, m_cubeMap->getTextureForContext(context));
}