#include "DepthMapPass.h"
#include "Shader.h"
#include "Regs.h"
#include "IRenderable.h"
#include "Matrix3D.h"
#include "IContext.h"
#include "Texture2DBase.h"
#include "Tex.h"

USING_AWAY_NAMESPACE

DepthMapPass::DepthMapPass()
{
	m_data[0] = 1;
	m_data[1] = 255;
	m_data[2] = 65025;
	m_data[3] = 16581375;
	m_data[4] = m_data[5] = m_data[6] = 1 / 255.f;
	m_data[7] = m_data[8] = m_data[9] = m_data[10] = m_data[11] = 0;
	m_alphaThreshold = 0;
}

void DepthMapPass::setAlphaThreshold(float value)
{
	assert(value >= 0 && value <= 1);
	if (value != m_alphaThreshold)
	{
		if (value == 0 || m_alphaThreshold == 0)
			invalidateShaderProgram();

		m_data[8] = m_alphaThreshold = value;

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

void DepthMapPass::getVertexCode(Shader& vertexShader)
{
	vertexShader.m44(Regs::t1, Regs::t0, Regs::c0);
	vertexShader.mov(Regs::op, Regs::t1);
	vertexShader.mov(Regs::v0, Regs::t1);

	if (m_alphaThreshold > 0)
		vertexShader.mov(Regs::v1, Regs::a1);
}

void DepthMapPass::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.div(Regs::t2, Regs::v0, Regs::v0 ^ Regs::w);
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

void DepthMapPass::render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection)
{
	if (m_alphaThreshold > 0)
		renderable->activateUVBuffer(1, context);

	Matrix3D matrix(renderable->getRenderSceneTransform(camera));
	matrix.append(viewProjection);
	context->setProgramConstantsFromMatrix(ProgramType::VERTEX, 0, matrix, true);
	renderable->activateVertexBuffer(0, context);
	context->drawTriangles(renderable->getIndexBuffer(context), 0, renderable->getTriangleCount());
}

void DepthMapPass::activate(IContext* context, Camera3D* camera)
{
	MaterialPassBase::activate(context, camera);

	if (m_alphaThreshold > 0)
	{
		bool useMipMaps = m_mipmap && m_alphaMask->hasMipMaps();
		context->setSamplerStateAt(0, m_repeat ? WrapMode::REPEAT : WrapMode::CLAMP, m_smooth ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
		context->setTextureAt(0, m_alphaMask->getTextureForContext(context));
		context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_data, 3);
	}
	else
	{
		context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_data, 2);
	}
}