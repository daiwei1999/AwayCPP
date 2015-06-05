#pragma once

#include "Common.h"
#include "ShaderCompiler.h"

AWAY_NAMESPACE_BEGIN

/**
 * SuperShaderCompiler is a compiler that generates shaders that perform both lighting and "effects" through methods.
 * This is used by the single-pass materials.
 */
class SuperShaderCompiler : public ShaderCompiler
{
public:
	SuperShaderCompiler();

protected:
	void initLightData() override;
	void calculateDependencies() override;
	void compileNormalCode() override;
	void createNormalRegisters() override;
	void compileViewDirCode() override;
	void compileLightingCode() override;

private:
	void compileTangentVertexCode(unsigned int matrixReg);
	void compileTangentNormalMapFragmentCode();
	void initLightRegisters();
	void compileDirectionalLightCode();
	void compilePointLightCode();
	void compileLightProbeCode();

private:
	std::vector<unsigned int> m_pointLightRegisters;
	std::vector<unsigned int> m_dirLightRegisters;
};

AWAY_NAMESPACE_END