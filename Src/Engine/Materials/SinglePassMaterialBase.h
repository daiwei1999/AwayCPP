#pragma once

#include "Common.h"
#include "MaterialBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * SinglePassMaterialBase forms an abstract base class for the default single-pass materials provided by Away3D,
 * using material methods to define their appearance.
 */
class SinglePassMaterialBase : public MaterialBase
{
public:
	SinglePassMaterialBase();
	~SinglePassMaterialBase();

	bool getLightFallOff();
	void setLightFallOff(bool value);
	float getAlphaThreshold();
	void setAlphaThreshold(float value);

	unsigned int getSpecularLightSources();
	void setSpecularLightSources(unsigned int value);
	unsigned int getDiffuseLightSources();
	void setDiffuseLightSources(unsigned int value);

	ColorTransform* getColorTransform();
	void setColorTransform(ColorTransform* value);
	BasicAmbientMethod* getAmbientMethod();
	void setAmbientMethod(BasicAmbientMethod* value);
	ShadowMapMethodBase* getShadowMethod();
	void setShadowMethod(ShadowMapMethodBase* value);
	BasicDiffuseMethod* getDiffuseMethod();
	void setDiffuseMethod(BasicDiffuseMethod* value);
	BasicNormalMethod* getNormalMethod();
	void setNormalMethod(BasicNormalMethod* value);
	BasicSpecularMethod* getSpecularMethod();
	void setSpecularMethod(BasicSpecularMethod* value);

	Texture2DBase* getNormalMap();
	void setNormalMap(Texture2DBase* value);
	Texture2DBase* getSpecularMap();
	void setSpecularMap(Texture2DBase* value);
	float getGloss();
	void setGloss(float value);
	float getAmbient();
	void setAmbient(float value);
	float getSpecular();
	void setSpecular(float value);
	unsigned int getAmbientColor();
	void setAmbientColor(unsigned int value);
	unsigned int getSpecularColor();
	void setSpecularColor(unsigned int value);
	bool getAlphaBlending() { return m_alphaBlending; }
	void setAlphaBlending(bool value);

	void setBlendMode(BlendMode value) override;
	void setDepthCompareMode(CompareMode value) override;
	void activateForDepth(IContext* context, Camera3D* camera, bool distanceBased = false) override;
	void updateMaterial(IContext* context) override;

	bool requiresBlending() override;
	void addMethod(EffectMethodBase* method);
	void addMethodAt(EffectMethodBase* method, int index);
	void removeMethod(EffectMethodBase* method);
	void removeAllMethod();
	int getMethodCount();
	bool hasMethod(EffectMethodBase* method);
	EffectMethodBase* getMethodAt(int index);

protected:
	SuperShaderPass* m_screenPass;

private:
	bool m_alphaBlending;
};

AWAY_NAMESPACE_END