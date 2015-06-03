#pragma once

#include "Common.h"
#include "ShadowMapMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * SimpleShadowMapMethodBase provides an abstract method for simple (non-wrapping) shadow map methods.
 */
class SimpleShadowMapMethodBase : public ShadowMapMethodBase
{
public:
	SimpleShadowMapMethodBase(LightBase* castingLight);

	void initVO(MethodVO* vo) override;
	void initConstants(MethodVO* vo) override;
	void cleanCompilationData() override;
	void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache) override;
	void getFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	void activate(MethodVO* vo, IContext* context) override;
	void setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera) override;

	virtual void getCascadeFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int decodeRegister, unsigned int depthTexture, unsigned int depthProjection, unsigned int targetRegister) = 0;
	virtual void activateForCascade(MethodVO* vo, IContext* context) = 0;

protected:
	void getPointVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache);
	void getPlanarVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache);
	virtual void getPlanarFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) = 0;
	virtual void getPointFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) = 0;

protected:
	unsigned int m_depthMapCoordReg;
	bool m_usePoint;
};

AWAY_NAMESPACE_END