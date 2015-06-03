#pragma once

#include "Common.h"
#include "CompiledPass.h"
#include "ShaderMethodSetup.h"
#include "ColorTransformMethod.h"

AWAY_NAMESPACE_BEGIN

/**
 * SuperShaderPass is a shader pass that uses shader methods to compile a complete program. It includes all methods
 * associated with a material.
 */
class SuperShaderPass : public CompiledPass
{
public:
	SuperShaderPass(MaterialBase* material);

	bool getIncludeCasters() { return m_includeCasters; }
	void setIncludeCasters(bool value);
	ColorTransform* getColorTransform() { return m_methodSetup->m_colorTransformMethod ? m_methodSetup->m_colorTransformMethod->getColorTransform() : nullptr; }
	void setColorTransform(ColorTransform* value);
	ColorTransformMethod* getColorTransformMethod() { return m_methodSetup->getColorTransformMethod(); }
	void setColorTransformMethod(ColorTransformMethod* value) { m_methodSetup->setColorTransformMethod(value); }

	void addMethod(EffectMethodBase* method) { m_methodSetup->addMethod(method); }
	void addMethodAt(EffectMethodBase* method, int index) { m_methodSetup->addMethodAt(method, index); }
	void removeMethod(EffectMethodBase* method) { m_methodSetup->removeMethod(method); }
	void removeAllMethod() { m_methodSetup->removeAllMethod(); }
	bool hasMethod(EffectMethodBase* method) { return m_methodSetup->hasMethod(method); }
	EffectMethodBase* getMethodAt(int index) { return m_methodSetup->getMethodAt(index); }
	int getMethodCount() { return m_methodSetup->getMethodCount(); }

	void activate(IContext* context, Camera3D* camera) override;
	void deactivate(IContext* context) override;

protected:
	ShaderCompiler* createCompiler() override;
	void updateLights() override;
	void addPassesFromMethods() override;
	void updateMethodConstants() override;
	void updateLightConstants() override;
	void updateProbes(IContext* context) override;

private:
	bool usesProbesForSpecular();
	bool usesProbesForDiffuse();

private:
	bool m_includeCasters;
};

AWAY_NAMESPACE_END