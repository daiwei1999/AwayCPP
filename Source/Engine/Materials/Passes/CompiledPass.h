#pragma once

#include "Common.h"
#include "MaterialPassBase.h"

AWAY_NAMESPACE_BEGIN

enum class LightSources
{
	LIGHTS = 1, // Defines normal lights are to be used as the source for the lighting component
	PROBES = 2, // Defines that global lighting probes are to be used as the source for the lighting component
	ALL = 3     // Defines that both normal and global lighting probes  are to be used as the source for the lighting component. This is equivalent to LIGHTS | PROBES
};

/**
 * CompiledPass forms an abstract base class for the default compiled pass materials provided by Away3D,
 * using material methods to define their appearance.
 */
class CompiledPass : public MaterialPassBase
{
	DEFINE_CALLBACK_CLASS(CompiledPass, onShaderInvalidated)

public:
	CompiledPass(MaterialBase* material);
	~CompiledPass();

	bool getLightFallOff() { return m_enableLightFallOff; }
	void setLightFallOff(bool value);
	bool getForceSeparateMVP() { return m_forceSeparateMVP; }
	void setForceSeparateMVP(bool value) { m_forceSeparateMVP = value; }
	bool getAnimateUVs() { return m_animateUVs; }
	void setAnimateUVs(bool value);
	Texture2DBase* getNormalMap();
	void setNormalMap(Texture2DBase* value);
	BasicNormalMethod* getNormalMethod();
	void setNormalMethod(BasicNormalMethod* value);
	BasicAmbientMethod* getAmbientMethod();
	void setAmbientMethod(BasicAmbientMethod* value);
	ShadowMapMethodBase* getShadowMethod();
	void setShadowMethod(ShadowMapMethodBase* value);
	BasicDiffuseMethod* getDiffuseMethod();
	void setDiffuseMethod(BasicDiffuseMethod* value);
	BasicSpecularMethod* getSpecularMethod();
	void setSpecularMethod(BasicSpecularMethod* value);
	unsigned int getSpecularLightSources() { return m_specularLightSources; }
	void setSpecularLightSources(unsigned int value) { m_specularLightSources = value; }
	unsigned int getDiffuseLightSources() { return m_diffuseLightSources; }
	void setDiffuseLightSources(unsigned int value) { m_diffuseLightSources = value; }
	bool getPassesDirty() { return m_passesDirty; }
	void setPassesDirty(bool value) { m_passesDirty = value; }

	MaterialPassVector& getPasses() { return m_passes; }
	int getPointLightCount() { return m_numPointLights; }
	int getDirectionalLightCount() { return m_numDirectionalLights; }
	int getLightProbeCount() { return m_numLightProbes; }

	void updateProgram(IContext* context) override;
	void invalidateShaderProgram(bool updateMaterial = true) override;
	void getVertexCode(Shader& vertexShader) override;
	void getFragmentCode(Shader& fragmentShader) override;
	void activate(IContext* context, Camera3D* camera) override;
	void deactivate(IContext* context) override;
	void render(IRenderable* renderable, IContext* context, Camera3D* camera, Matrix3D& viewProjection) override;

protected:
	virtual ShaderCompiler* createCompiler() = 0;
	virtual void addPassesFromMethods();
	bool usesProbes();
	bool usesLights();
	void initCompiler();
	void updateShaderProperties();
	void updateRegisterIndices();
	void addPasses(MaterialPassVector& passes);
	void cleanUp();
	virtual void updateMethodConstants();
	virtual void updateLightConstants();
	virtual void updateProbes(IContext* context);

private:
	void updateUsedOffsets();
	void initConstantData();
	void initUVTransformData();
	void initCommonsData();
	void onShaderInvalidated(Event* event);

protected:
	MaterialPassVector m_passes;
	unsigned int m_specularLightSources;
	unsigned int m_diffuseLightSources;
	ShaderChunk* m_vertexCode;
	ShaderChunk* m_fragmentLightCode;
	ShaderChunk* m_framentPostLightCode;
	std::vector<float> m_vertexConstantData;
	std::vector<float> m_fragmentConstantData;
	int m_commonsDataIndex;
	int m_probeWeightsIndex;
	int m_uvBufferIndex;
	int m_secondaryUVBufferIndex;
	int m_normalBufferIndex;
	int m_tangentBufferIndex;
	int m_sceneMatrixIndex;
	int m_sceneNormalMatrixIndex;
	int m_lightFragmentConstantIndex;
	int m_cameraPositionIndex;
	int m_uvTransformIndex;
	std::vector<unsigned int>* m_lightProbeDiffuseIndices;
	std::vector<unsigned int>* m_lightProbeSpecularIndices;
	float m_ambientLightR, m_ambientLightG, m_ambientLightB;
	ShaderCompiler* m_compiler;
	ShaderMethodSetup* m_methodSetup;
	bool m_usingSpecularMethod;
	bool m_usesNormals;
	bool m_animateUVs;
	int m_numPointLights;
	int m_numDirectionalLights;
	int m_numLightProbes;
	bool m_enableLightFallOff;
	bool m_passesDirty;

private:
	bool m_forceSeparateMVP;
};

AWAY_NAMESPACE_END