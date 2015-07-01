#include "MaterialPassBase.h"
#include "LightPickerBase.h"
#include "IContext.h"
#include "Regs.h"
#include "Shader.h"
#include "AGALProgramCache.h"
#include "IRenderable.h"
#include "IAnimator.h"
#include "IAnimationSet.h"
#include "Event.h"
#include "MaterialBase.h"

USING_AWAY_NAMESPACE

int MaterialPassBase::m_previousUsedStreams = 0;
int MaterialPassBase::m_previousUsedTexs = 0;

MaterialPassBase::MaterialPassBase(bool renderToTexture)
{
	m_programId = -1;
	m_program = nullptr;
	m_animationRegisterCache = nullptr;
	m_material = nullptr;
	m_animationSet = nullptr;
	m_smooth = true;
	m_repeat = false;
	m_mipmap = true;
	m_enableBlending = false;
	m_depthCompareMode = CompareMode::LESS_EQUAL;
	m_blendFactorSource = BlendFactor::ONE;
	m_blendFactorDest = BlendFactor::ZERO;
	m_lightPicker = nullptr;
	
	m_context = nullptr;
	m_bothSides = false;
	m_animatableAttributes.push_back(Regs::a0);
	m_animationTargetRegisters.push_back(Regs::t0);
	m_shadedTarget = Regs::t0;
	m_renderToTexture = renderToTexture;
	m_numUsedStreams = 1;
	m_numUsedTextures = 0;
	m_numUsedVertexConstants = 4;
	m_numUsedFragmentConstants = 0;
	m_numUsedVaryings = 0;

	m_alphaPremultiplied = false;
	m_needFragmentAnimation = false;
	m_needUVAnimation = false;
}

MaterialPassBase::~MaterialPassBase()
{

}

void MaterialPassBase::setMipmap(bool value)
{
	if (value != m_mipmap)
	{
		m_mipmap = value;
		invalidateShaderProgram();
	}
}

void MaterialPassBase::setSmooth(bool value)
{
	if (value != m_smooth)
	{
		m_smooth = value;
		invalidateShaderProgram();
	}
}

void MaterialPassBase::setRepeat(bool value)
{
	if (value != m_repeat)
	{
		m_repeat = value;
		invalidateShaderProgram();
	}
}

void MaterialPassBase::setAlphaPremultiplied(bool value)
{
	if (value != m_alphaPremultiplied)
	{
		m_alphaPremultiplied = value;
		invalidateShaderProgram(false);
	}
}

void MaterialPassBase::setAnimationSet(IAnimationSet* value)
{
	if (value != m_animationSet)
	{
		m_animationSet = value;
		invalidateShaderProgram();
	}
}

void MaterialPassBase::setLightPicker(LightPickerBase* value)
{
	if (m_lightPicker == value)
		return;

	if (m_lightPicker)
		m_lightPicker->removeEventListener(Event::CHANGE, CALLBACK_OBJECT(onLightsChange));

	m_lightPicker = value;

	if (m_lightPicker)
		m_lightPicker->addEventListener(Event::CHANGE, CALLBACK_OBJECT(onLightsChange));

	updateLights();
}

void MaterialPassBase::updateAnimationState(IRenderable* renderable, IContext* context, Camera3D* camera)
{
	renderable->getAnimator()->setRenderState(context, renderable, m_numUsedVertexConstants, m_numUsedStreams, camera);
}

void MaterialPassBase::setBlendMode(BlendMode value)
{
	switch (value)
	{
	case BlendMode::NORMAL:
		m_blendFactorSource = BlendFactor::ONE;
		m_blendFactorDest = BlendFactor::ZERO;
		m_enableBlending = false;
		break;
	case BlendMode::LAYER:
		m_blendFactorSource = BlendFactor::SOURCE_ALPHA;
		m_blendFactorDest = BlendFactor::ONE_MINUS_SOURCE_ALPHA;
		m_enableBlending = true;
		break;
	case BlendMode::MULTIPLY:
		m_blendFactorSource = BlendFactor::ZERO;
		m_blendFactorDest = BlendFactor::SOURCE_COLOR;
		m_enableBlending = true;
		break;
	case BlendMode::ADD:
		m_blendFactorSource = BlendFactor::SOURCE_ALPHA;
		m_blendFactorDest = BlendFactor::ONE;
		m_enableBlending = true;
		break;
	case BlendMode::ALPHA:
		m_blendFactorSource = BlendFactor::ZERO;
		m_blendFactorDest = BlendFactor::SOURCE_ALPHA;
		m_enableBlending = true;
		break;
	case BlendMode::SCREEN:
		m_blendFactorSource = BlendFactor::ONE;
		m_blendFactorDest = BlendFactor::ONE_MINUS_SOURCE_COLOR;
		m_enableBlending = true;
		break;
	}
}

void MaterialPassBase::activate(IContext* context, Camera3D* camera)
{
	context->setDepthTest(!m_enableBlending, m_depthCompareMode);
	if (m_enableBlending)
		context->setBlendFactors(m_blendFactorSource, m_blendFactorDest);

	if (m_context != context || !m_program)
	{
		m_context = context;
		updateProgram(context);

		Event event(Event::CHANGE);
		dispatchEvent(&event);
	}

	for (int i = m_numUsedStreams; i < m_previousUsedStreams; i++)
		context->setVertexBufferAt(i, nullptr);

	for (int i = m_numUsedTextures; i < m_previousUsedTexs; i++)
		context->setTextureAt(i, nullptr);

	if (m_animationSet && !m_animationSet->usesCPU())
		m_animationSet->activate(context, this);

	context->setProgram(m_program);
	context->setCulling(m_bothSides ? TriangleFace::NONE : TriangleFace::BACK);

	if (m_renderToTexture)
	{

	}
}

void MaterialPassBase::deactivate(IContext* context)
{
	m_previousUsedStreams = m_numUsedStreams;
	m_previousUsedTexs = m_numUsedTextures;

	if (m_animationSet && !m_animationSet->usesCPU())
		m_animationSet->deactivate(context, this);

	if (m_renderToTexture)
	{

	}

	context->setDepthTest(true, CompareMode::LESS_EQUAL);
}

void MaterialPassBase::invalidateShaderProgram(bool updateMaterial)
{
	m_program = nullptr;

	if (m_material && updateMaterial)
		m_material->invalidatePasses(this);
}

void MaterialPassBase::updateProgram(IContext* context)
{
	Shader vertexShader(ProgramType::VERTEX);
	Shader fragmentShader(ProgramType::FRAGMENT);

	if (m_animationSet && !m_animationSet->usesCPU())
	{
		m_animationSet->getAGALVertexCode(vertexShader, this, m_animatableAttributes, m_animationTargetRegisters, m_uvSource, m_uvTarget);

		if (m_needFragmentAnimation)
			m_animationSet->getAGALFragmentCode(fragmentShader, this, m_shadedTarget);

		if (m_needUVAnimation)
			m_animationSet->getAGALUVCode(vertexShader, this, m_uvSource, m_uvTarget);

		m_animationSet->doneAGALCode(this);
	}
	else
	{
		// simply write attributes to targets, do not animate them
		// projection will pick up on targets[0] to do the projection
		std::size_t len = m_animatableAttributes.size();
		for (std::size_t i = 0; i < len; i++)
			vertexShader.mov(m_animationTargetRegisters[i], m_animatableAttributes[i]);

		if (m_needUVAnimation)
			vertexShader.mov(m_uvTarget, m_uvSource);
	}

	getVertexCode(vertexShader);
	getFragmentCode(fragmentShader);

	AGALProgramCache::getInstance(context)->setProgram(this, vertexShader, fragmentShader);
}

void MaterialPassBase::updateLights()
{

}

void MaterialPassBase::onLightsChange(Event* event)
{
	updateLights();
}