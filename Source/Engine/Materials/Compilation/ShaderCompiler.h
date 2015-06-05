#pragma once

#include "Common.h"
#include "ShaderChunk.h"
#include "ShaderRegisterData.h"
#include "ShaderRegisterCache.h"
#include "MethodDependencyCounter.h"

AWAY_NAMESPACE_BEGIN

/**
 * ShaderCompiler is an abstract base class for shader compilers that use modular shader methods to assemble a
 * material. Concrete subclasses are used by the default materials.
 */
class ShaderCompiler
{
public:
	ShaderCompiler();
	virtual ~ShaderCompiler();

	bool getLightFallOff() { return m_enableLightFallOff; }
	void setLightFallOff(bool value) { m_enableLightFallOff = value; }
	bool needUVAnimation() { return m_needUVAnimation; }
	unsigned int getUVTarget() { return m_uvTarget; }
	unsigned int getUVSource() { return m_uvSource; }
	bool getForceSeperateMVP() { return m_forceSeperateMVP; }
	void setForceSeperateMVP(bool value) { m_forceSeperateMVP = value; }
	bool getAnimateUVs() { return m_animateUVs; }
	void setAnimateUVs(bool value) { m_animateUVs = value; }
	bool getAlphaPremultiplied() { return m_alphaPremultiplied; }
	void setAlphaPremultiplied(bool value) { m_alphaPremultiplied = value; }
	unsigned int getSpecularLightSources() { return m_specularLightSources; }
	void setSpecularLightSources(unsigned int value) { m_specularLightSources = value; }
	unsigned int getDiffuseLightSources() { return m_diffuseLightSources; }
	void setDiffuseLightSources(unsigned int value) { m_diffuseLightSources = value; }
	ShaderMethodSetup* getMethodSetup() { return m_methodSetup; }
	void setMethodSetup(ShaderMethodSetup* value) { m_methodSetup = value; }
	int getPointLightCount() { return m_numPointLights; }
	void setPointLightCount(int value) { m_numPointLights = value; }
	int getDirectionalLightCount() { return m_numDirectionalLights; }
	void setDirectionalLightCount(int value) { m_numDirectionalLights = value; }
	int getLightProbeCount() { return m_numLightProbes; }
	void setLightProbeCount(int value) { m_numLightProbes = value; }

	int getCommonsDataIndex() { return m_commonsDataIndex; }
	int getUsedVertexConstantCount() { return m_registerCache.getUsedVertexConstantCount(); }
	int getUsedFragmentConstantCount() { return m_registerCache.getUsedFragmentConstantCount(); }
	int getUsedStreamCount() { return m_registerCache.getUsedStreamCount(); }
	int getUsedTextureCount() { return m_registerCache.getUsedTextureCount(); }
	int getUsedVaryingCount() { return m_registerCache.getUsedVaryingCount(); }
	int getUVBufferIndex() { return m_uvBufferIndex; }
	int getUVTransformIndex() { return m_uvTransformIndex; }
	int getSecondaryUVBufferIndex() { return m_secondaryUVBufferIndex; }
	int getNormalBufferIndex() { return m_normalBufferIndex; }
	int getTangentBufferIndex() { return m_tangentBufferIndex; }
	int getLightFragmentConstantIndex() { return m_lightFragmentConstantIndex; }
	int getCameraPositionIndex() { return m_cameraPositionIndex; }
	int getSceneMatrixIndex() { return m_sceneMatrixIndex; }
	int getSceneNormalMatrixIndex() { return m_sceneNormalMatrixIndex; }
	int getProbeWeightsIndex() { return m_probeWeightsIndex; }
	ShaderChunk* getVertexCode() { return &m_vertexCode; }
	ShaderChunk* getFragmentLightCode() { return &m_fragmentLightCode; }
	ShaderChunk* getFragmentPostLightCode() { return &m_fragmentPostLightCode; }
	unsigned int getShadedTarget() { return m_sharedRegisters.m_shadedTarget; }
	bool usingSpecularMethod() { return m_usingSpecularMethod; }
	std::vector<unsigned int>& getAnimatableAttributes() { return m_animatableAttributes; }
	std::vector<unsigned int>& getAnimationTargetRegisters() { return m_animationTargetRegisters; }
	std::vector<unsigned int>* getLightProbeDiffuseIndices() { return &m_lightProbeDiffuseIndices; }
	std::vector<unsigned int>* getLightProbeSpecularIndices() { return &m_lightProbeSpecularIndices; }

	void setTextureSampling(bool smooth, bool repeat, bool mipmap);
	void setConstantDataBuffers(std::vector<float>* vertexConstantData, std::vector<float>* fragmentConstantData);
	bool usesNormals();
	bool usesLights();
	void compile();

protected:
	virtual void createNormalRegisters();
	virtual void compileLightingCode();
	virtual void compileViewDirCode();
	virtual void compileNormalCode();
	void initRegisterIndices();
	virtual void initLightData();
	virtual void calculateDependencies();
	
	bool usesLightsForSpecular();
	bool usesLightsForDiffuse();
	bool usesProbesForSpecular();
	bool usesProbesForDiffuse();
	bool usesProbes();

private:
	void compileMethods();
	void compileMethodsCode();
	void compileUVCode();
	void compileSecondaryUVCode();
	void compileGlobalPositionCode();
	void compileProjectionCode();
	void compileFragmentOutput();
	void createCommons();
	void setupAndCountMethodDependencies(ShadingMethodBase* method, MethodVO* methodVO);
	void setupMethod(ShadingMethodBase* method, MethodVO* methodVO);
	void updateMethodRegisters();
	void cleanUpMethods();

protected:
	ShaderRegisterData m_sharedRegisters;
	ShaderRegisterCache m_registerCache;
	MethodDependencyCounter m_dependencyCounter;
	ShaderMethodSetup* m_methodSetup;

	bool m_smooth;
	bool m_repeat;
	bool m_mipmap;
	bool m_enableLightFallOff;
	bool m_animateUVs;
	bool m_alphaPremultiplied;
	std::vector<float>* m_vertexConstantData;
	std::vector<float>* m_fragmentConstantData;
	ShaderChunk m_vertexCode; // The generated vertex code
	ShaderChunk m_fragmentLightCode; // The code containing the lighting calculations
	ShaderChunk m_fragmentPostLightCode; // The code containing the post-lighting calculations
	
	std::vector<unsigned int> m_animatableAttributes;
	std::vector<unsigned int> m_animationTargetRegisters;
	std::vector<unsigned int> m_lightProbeDiffuseIndices;
	std::vector<unsigned int> m_lightProbeSpecularIndices;
	int m_uvBufferIndex;
	int m_uvTransformIndex;
	int m_secondaryUVBufferIndex;
	int m_normalBufferIndex;
	int m_tangentBufferIndex;
	int m_lightFragmentConstantIndex;
	int m_sceneMatrixIndex;
	int m_sceneNormalMatrixIndex;
	int m_cameraPositionIndex;
	int m_probeWeightsIndex;
	unsigned int m_specularLightSources;
	unsigned int m_diffuseLightSources;

	int m_numLights;
	int m_numLightProbes;
	int m_numPointLights;
	int m_numDirectionalLights;
	int m_numProbeRegisters;
	unsigned int m_combinedLightSources;
	bool m_usingSpecularMethod;
	bool m_needUVAnimation;
	bool m_forceSeperateMVP;
	unsigned int m_uvTarget, m_uvSource;

private:
	int m_commonsDataIndex;
};

AWAY_NAMESPACE_END