#include "SimpleShadowMapMethodBase.h"
#include "PointLight.h"
#include "MethodVO.h"
#include "DirectionalShadowMapper.h"
#include "ShaderRegisterCache.h"
#include "ShaderChunk.h"
#include "ShaderRegisterData.h"
#include "Regs.h"
#include "IContext.h"
#include "TextureProxyBase.h"

USING_AWAY_NAMESPACE

SimpleShadowMapMethodBase::SimpleShadowMapMethodBase(LightBase* castingLight) : ShadowMapMethodBase(castingLight)
{
	m_usePoint = castingLight->getType() == LightType::Point;
}

void SimpleShadowMapMethodBase::initVO(MethodVO* vo)
{
	vo->m_needsGlobalVertexPos = !m_usePoint;
	vo->m_needsNormals = vo->m_numLights > 0;
}

void SimpleShadowMapMethodBase::initConstants(MethodVO* vo)
{
	std::vector<float>& vertexData = *(vo->m_vertexData);
	int index = vo->m_vertexConstantsIndex;
	if (index != -1)
	{
		vertexData[index] = 0.5f;
		vertexData[index + 1] = -0.5f;
		vertexData[index + 2] = 0;
		vertexData[index + 3] = 1;
	}

	std::vector<float>& fragmentData = *vo->m_fragmentData;
	index = vo->m_fragmentConstantsIndex;
	fragmentData[index] = 1;
	fragmentData[index + 1] = 1 / 255.f;
	fragmentData[index + 2] = 1 / 65025.f;
	fragmentData[index + 3] = 1 / 16581375.f;
	fragmentData[index + 6] = 0;
	fragmentData[index + 7] = 1;

	if (m_usePoint)
	{
		fragmentData[index + 8] = 0;
		fragmentData[index + 9] = 0;
		fragmentData[index + 10] = 0;
		fragmentData[index + 11] = 1;
	}
}

void SimpleShadowMapMethodBase::cleanCompilationData()
{
	ShadowMapMethodBase::cleanCompilationData();
	m_depthMapCoordReg = 0;
}

void SimpleShadowMapMethodBase::getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	if (m_usePoint)
		getPointVertexCode(code, vo, regCache);
	else
		getPlanarVertexCode(code, vo, regCache);
}

void SimpleShadowMapMethodBase::getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg)
{
	if (m_usePoint)
		getPointFragmentCode(code, vo, regCache, targetReg);
	else
		getPlanarFragmentCode(code, vo, regCache, targetReg);

	code.add(targetReg ^ Regs::w, targetReg ^ Regs::w, (Regs::c0 + (vo->m_fragmentConstantsIndex / 4 + 1)) ^ Regs::y);
	code.sat(targetReg ^ Regs::w, targetReg);
}

void SimpleShadowMapMethodBase::activate(MethodVO* vo, IContext* context)
{
	std::vector<float>& fragmentData = *vo->m_fragmentData;
	int index = vo->m_fragmentConstantsIndex;

	if (m_usePoint)
	{
		float fallOff = static_cast<PointLight*>(m_castingLight)->getFallOff();
		fragmentData[index + 4] = -std::powf(1.f / (fallOff * m_epsilon), 2);

		Vector3D* lightPos = m_castingLight->getScenePosition();
		fragmentData[index + 8] = lightPos->m_x;
		fragmentData[index + 9] = lightPos->m_y;
		fragmentData[index + 10] = lightPos->m_z;
		fragmentData[index + 11] = 1 / (2 * fallOff * fallOff); // used to decompress distance
	}
	else
		(*vo->m_vertexData)[vo->m_vertexConstantsIndex + 3] = -1 / (static_cast<DirectionalShadowMapper*>(m_shadowMapper)->getDepth() * m_epsilon);

	fragmentData[index + 5] = 1 - m_alpha;

	context->setSamplerStateAt(vo->m_texturesIndex, WrapMode::CLAMP, TextureFilter::NEAREST, MipFilter::MIPNONE);
	context->setTextureAt(vo->m_texturesIndex, m_castingLight->getShadowMapper()->getDepthMap()->getTextureForContext(context));
}

void SimpleShadowMapMethodBase::setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera)
{
	if (!m_usePoint)
		static_cast<DirectionalShadowMapper*>(m_shadowMapper)->getDepthProjection().copyRawDataTo(vo->m_vertexData->data(), vo->m_vertexConstantsIndex + 4, true);
}

void SimpleShadowMapMethodBase::getPointVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	vo->m_vertexConstantsIndex = -1;
}

void SimpleShadowMapMethodBase::getPlanarVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache)
{
	unsigned int temp = regCache->getFreeVertexVectorTemp();
	unsigned int dataReg = regCache->getFreeVertexConstant();
	vo->m_vertexConstantsIndex = REGISTER_INDEX(dataReg) * 4;

	unsigned int depthProjMatrix = regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();
	regCache->getFreeVertexConstant();

	m_depthMapCoordReg = regCache->getFreeVarying();
	
	// todo: can epsilon be applied here instead of fragment shader?
	code.m44(temp, m_sharedRegisters->m_globalPositionVertex, depthProjMatrix);
	code.div(temp, temp, temp ^ Regs::w);
	code.mul(temp ^ Regs::xy, temp ^ Regs::xy, dataReg ^ Regs::xy);
	code.add(m_depthMapCoordReg, temp, dataReg ^ Regs::xxwz);
}