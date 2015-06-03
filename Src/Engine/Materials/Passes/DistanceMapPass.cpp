#include "DistanceMapPass.h"
#include "Shader.h"
#include "Regs.h"
#include "Camera3D.h"
#include "LensBase.h"
#include "IRenderable.h"
#include "IContext.h"
#include "Texture2DBase.h"
#include "Tex.h"

USING_AWAY_NAMESPACE

DistanceMapPass::DistanceMapPass()
{
	m_fragmentData[0] = 1;
	m_fragmentData[1] = 255;
	m_fragmentData[2] = 65025;
	m_fragmentData[3] = 16581375;
	m_fragmentData[4] = m_fragmentData[5] = m_fragmentData[6] = 1 / 255.f;
	m_fragmentData[7] = m_fragmentData[8] = m_fragmentData[9] = m_fragmentData[10] = m_fragmentData[11] = 0;
	m_vertexData[3] = 1;
	m_numUsedVertexConstants = 9;
}

void DistanceMapPass::setAlphaThreshold(float value)
{
	assert(value >= 0 && value <= 1);
	if (value != m_alphaThreshold)
	{
		if (value == 0 || m_alphaThreshold == 0)
			invalidateShaderProgram();

		m_fragmentData[8] = m_alphaThreshold = value;

		if (m_alphaThreshold > 0)
		{
			m_numUsedTextures = 1;
			m_numUsedStreams = 2;
		}
		else
		{
			m_numUsedTextures = 0;
			m_numUsedStreams = 1;
		}
	}
}

void DistanceMapPass::getVertexCode(Shader& vertexShader)
{
	vertexShader.m44(Regs::op, Regs::t0, Regs::c0);
	vertexShader.m44(Regs::t1, Regs::t0, Regs::c4);
	vertexShader.sub(Regs::v0, Regs::t1, Regs::c8);

	if (m_alphaThreshold > 0)
		vertexShader.mov(Regs::v1, Regs::a1);
}

void DistanceMapPass::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.dp3(Regs::t2 ^ Regs::z, Regs::v0, Regs::v0);
	fragmentShader.mul(Regs::t0, Regs::c0, Regs::t2 ^ Regs::z);
	fragmentShader.frc(Regs::t0, Regs::t0);
	fragmentShader.mul(Regs::t1, Regs::t0 ^ Regs::yzw, Regs::c1);

	if (m_alphaThreshold > 0)
	{
		unsigned int format;
		switch (m_alphaMask->getFormat())
		{
		case TextureFormat::COMPRESSED:
			format = Tex::DXT1;
			break;
		case TextureFormat::COMPRESSED_ALPHA:
			format = Tex::DXT5;
			break;
		default:
			format = Tex::RGBA;
		}
		fragmentShader.tex(Regs::t3, Regs::v1, Regs::s0 | format);
		fragmentShader.sub(Regs::t3 ^ Regs::w, Regs::t3 ^ Regs::w, Regs::c2 ^ Regs::x);
		fragmentShader.kil(Regs::t3 ^ Regs::w);
	}
	fragmentShader.sub(Regs::oc, Regs::t0, Regs::t1);
}

void DistanceMapPass::render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	Vector3D* cameraPos = camera->getScenePosition();
	m_vertexData[0] = cameraPos->m_x;
	m_vertexData[1] = cameraPos->m_y;
	m_vertexData[2] = cameraPos->m_z;

	Matrix3D& sceneTransform = renderable->getRenderSceneTransform(camera);
	context->setProgramConstantsFromMatrix(ProgramType::VERTEX, 4, sceneTransform, true);
	context->setProgramConstantsFromVector(ProgramType::VERTEX, 8, m_vertexData, 1);

	if (m_alphaThreshold > 0)
		renderable->activateUVBuffer(1, context);

	Matrix3D matrix(sceneTransform);
	matrix.append(viewProjection);
	context->setProgramConstantsFromMatrix(ProgramType::VERTEX, 0, matrix, true);
	renderable->activateVertexBuffer(0, context);
	context->drawTriangles(renderable->getIndexBuffer(context), 0, renderable->getTriangleCount());
}

void DistanceMapPass::activate(IContext* context, Camera3D* camera)
{
	MaterialPassBase::activate(context, camera);

	float f = camera->getLens()->getFar();
	f = 1 / (2 * f * f);
	m_fragmentData[0] = f;
	m_fragmentData[1] = 255 * f;
	m_fragmentData[2] = 65025 * f;
	m_fragmentData[3] = 16581375 * f;

	if (m_alphaThreshold > 0)
	{
		bool useMipMaps = m_mipmap && m_alphaMask->hasMipMaps();
		context->setSamplerStateAt(0, m_repeat ? WrapMode::REPEAT : WrapMode::CLAMP, m_smooth ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(0, m_alphaMask->getTextureForContext(context));
		context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_fragmentData, 3);
	}
	else
	{
		context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_fragmentData, 2);
	}
}