#pragma once

#include "Common.h"
#include "EventDispatcher.h"

AWAY_NAMESPACE_BEGIN

/**
 * ShadingMethodBase provides an abstract base method for shading methods, used by compiled passes to compile
 * the final shading program.
 */
class ShadingMethodBase : public EventDispatcher
{
public:
	ShadingMethodBase();

	ShaderRegisterData* getSharedRegisters() { return m_sharedRegisters; }
	void setSharedRegisters(ShaderRegisterData* value) { m_sharedRegisters = value; }

	MaterialPassVector& getPasses() { return m_passes; }
	MethodVO* createMethodVO();

	virtual void initVO(MethodVO* vo);
	virtual void initConstants(MethodVO* vo);
	virtual void reset();
	virtual void cleanCompilationData();
	virtual void getVertexCode(ShaderChunk& code, MethodVO* vo, ShaderRegisterCache* regCache);
	virtual void activate(MethodVO* vo, IContext* context);
	virtual void setRenderState(MethodVO* vo, IRenderable* renderable, IContext* context, Camera3D* camera);
	virtual void deactivate(MethodVO* vo, IContext* context);
	virtual void copyFrom(ShadingMethodBase* method);

protected:
	void getTex2DSampleCode(ShaderChunk& code, unsigned int targetReg, unsigned int inputReg, TextureProxyBase* texture, unsigned int uvReg);
	void getTexCubeSampleCode(ShaderChunk& code, unsigned int targetReg, unsigned int inputReg, TextureProxyBase* texture, unsigned int uvReg);
	void invalidateShaderProgram();

private:
	unsigned int getFormatForTexture(TextureProxyBase* texture);

protected:
	ShaderRegisterData* m_sharedRegisters;
	MaterialPassVector m_passes;
};

AWAY_NAMESPACE_END