#include "ProjectiveTextureMethod.h"
#include "TextureProjector.h"
#include "MethodVO.h"
#include "IContext.h"
#include "Regs.h"
#include "Matrix3D.h"
#include "IRenderable.h"
#include "ShaderChunk.h"
#include "ShaderRegisterCache.h"
#include "Texture2DBase.h"

USING_AWAY_NAMESPACE

ProjectiveTextureMethod::ProjectiveTextureMethod(TextureProjector* projector, ProjectiveMode mode, bool smooth)
{
	m_projector = projector;
	m_mode = mode;
	m_smooth = smooth;
}

void ProjectiveTextureMethod::setMode(ProjectiveMode value)
{
	if (value != m_mode)
	{
		m_mode = value;
		invalidateShaderProgram();
	}
}

void ProjectiveTextureMethod::initConstants(MethodVO* vo)
{
	std::vector<float>& data = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;
	data[index] = .5f;
	data[index + 1] = -.5f;
	data[index + 2] = 1;
	data[index + 3] = 1;
}

void ProjectiveTextureMethod::activate(MethodVO* vo, IContext* context)
{
	Texture2DBase* texture = m_projector->getTexture();
	bool useMipMaps = vo->m_useMipmapping && texture->hasMipMaps();
	context->setSamplerStateAt(vo->m_texturesIndex, WrapMode::CLAMP, m_smooth ? TextureFilter::LINEAR : TextureFilter::NEAREST, useMipMaps ? MipFilter::MIPLINEAR : MipFilter::MIPNONE);
	context->setTextureAt(vo->m_texturesIndex, texture->getTextureForContext(context));
}

void ProjectiveTextureMethod::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	unsigned int projReg = regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	vo->m_vertexConstantsIndex = REGISTER_INDEX(projReg) * 4;

	m_uvVarying = regCache->getFreeVarying();

	code.m44(m_uvVarying, Regs::t0, projReg);
}

void ProjectiveTextureMethod::setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera)
{
	Matrix3D projMatrix;
	projMatrix.copyFrom(m_projector->getViewProjection());
	projMatrix.prepend(renderable->getRenderSceneTransform(camera));
	projMatrix.copyRawDataTo(vo->m_vertexData->data(), vo->m_vertexConstantsIndex, true);
}

void ProjectiveTextureMethod::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	unsigned int mapRegister = regCache->getFreeTextureReg();
	unsigned int toTexReg = regCache->getFreeFragmentConstant();
	vo->m_texturesIndex = REGISTER_INDEX(mapRegister);
	vo->m_fragmentConstantsIndex = REGISTER_INDEX(toTexReg) * 4;

	unsigned int temp = regCache->getFreeFragmentVectorTemp();
	code.div(temp, m_uvVarying, m_uvVarying ^ Regs::w);
	code.mul(temp ^ Regs::xy, temp ^ Regs::xy, toTexReg ^ Regs::xy);
	code.add(temp ^ Regs::xy, temp ^ Regs::xy, toTexReg ^ Regs::x);
	getTex2DSampleCode(code, temp, mapRegister, m_projector->getTexture(), temp);

	switch (m_mode)
	{
	case ProjectiveMode::ADD:
		code.add(targetReg ^ Regs::xyz, targetReg, temp);
		break;
	case ProjectiveMode::MULTIPLY:
		code.mul(targetReg ^ Regs::xyz, targetReg, temp);
		break;
	case ProjectiveMode::MIX:
		code.sub(temp ^ Regs::xyz, temp, targetReg);
		code.mul(temp ^ Regs::xyz, temp, temp ^ Regs::w);
		code.add(targetReg ^ Regs::xyz, targetReg, temp);
		break;
	}
}