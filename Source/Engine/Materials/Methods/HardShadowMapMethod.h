#pragma once 

#include "Common.h"
#include "SimpleShadowMapMethodBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * HardShadowMapMethod provides the cheapest shadow map method by using a single tap without any filtering.
 */
class HardShadowMapMethod : public SimpleShadowMapMethodBase
{
public:
	HardShadowMapMethod(LightBase* castingLight);

	void initVO(MethodVO* vo) override;
	void getCascadeFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int decodeRegister, unsigned int depthTexture, unsigned int depthProjection, unsigned int targetRegister) override;
	void activateForCascade(MethodVO* vo, IContext* context) override;

protected:
	void getPlanarFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
	void getPointFragmentCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache, unsigned int targetReg) override;
};

AWAY_NAMESPACE_END