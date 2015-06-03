#pragma once

#include "Common.h"
#include "IContext.h"

AWAY_NAMESPACE_BEGIN

class TextureState
{
public:
	int m_wrap;
	int m_magFilter;
	int m_minfilter;
	TextureBase* m_texture = nullptr;
};

class OpenGLES2Context : public IContext
{
public:
	OpenGLES2Context();
	~OpenGLES2Context();

	const std::string& driverInfo() override { return m_driverInfo; }

	void clear(float red = 0, float green = 0, float blue = 0, float alpha = 1, float depth = 1, unsigned int stencil = 0, ClearMask mask = ClearMask::ALL) override;
	void configureViewport(int x, int y, int width, int height, bool enableStencil = true) override;

	Texture* createTexture(int width, int height, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels = 0) override;
	CubeTexture* createCubeTexture(int size, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels = 0) override;

	IndexBuffer* createIndexBuffer(int numIndices, BufferUsage bufferUsage = BufferUsage::STATIC_DRAW) override;
	VertexBuffer* createVertexBuffer(int numVertices, int bytesPerVertex, BufferUsage bufferUsage = BufferUsage::STATIC_DRAW) override;

	Program* createProgram() override;

	void drawLines(IndexBuffer* indexBuffer, int firstIndex, int numLines) override;
	void drawTriangles(IndexBuffer* indexBuffer, int firstIndex, int numTriangles) override;
	void present() override;

	void setBlendFactors(BlendFactor sourceFactor, BlendFactor destinationFactor) override;
	void setColorMask(bool red, bool green, bool blue, bool alpha) override;
	void setCulling(TriangleFace triangleFaceToCull) override;
	void setDepthTest(bool depthMask, CompareMode passCompareMode) override;
	void setFillMode(FillMode fillMode) override;
	void setProgram(Program* program) override;

	void setProgramConstantsFromMatrix(ProgramType programType, int firstRegister, Matrix3D& matrix, bool transposedMatrix = false) override;
	void setProgramConstantsFromVector(ProgramType programType, int firstRegister, float* data, int numRegisters) override;

	void setRenderToBackBuffer() override;
	void setRenderToTexture(TextureBase* texture, bool enableDepthAndStencil = false, int antiAlias = 0, int surfaceSelector = 0, int colorOutputIndex = 0) override;
	void setSamplerStateAt(int sampler, WrapMode wrap, TextureFilter filter, MipFilter mipfilter) override;
	void setScissorRectangle(Rectangle* rectangle) override;
	void setStencilActions(TriangleFace triangleFace = TriangleFace::FRONT_AND_BACK,
						   CompareMode compareMode = CompareMode::ALWAYS,
						   StencilAction actionOnBothPass = StencilAction::KEEP,
						   StencilAction actionOnDepthFail = StencilAction::KEEP,
						   StencilAction actionOnDepthPassStencilFail = StencilAction::KEEP) override;
	void setStencilReferenceValue(int referenceValue, unsigned int readMask, unsigned int writeMask) override;
	void setTextureAt(int sampler, TextureBase* texture) override;
	void setVertexBufferAt(int index, VertexBuffer* buffer, int offset = 0, VertexBufferFormat format = VertexBufferFormat::FLOAT_4) override;

private:
	typedef std::map<TriangleFace, unsigned int> TriangleFaceMap;
	typedef std::map<BlendFactor, unsigned int> BlendFactorMap;
	typedef std::map<CompareMode, unsigned int> CompareFuncMap;
	typedef std::map<StencilAction, unsigned int> StencilActionMap;
	
	static TriangleFaceMap m_triangleFaces;
	static BlendFactorMap m_blendFactors;
	static CompareFuncMap m_compareFuncs;
	static StencilActionMap m_stencilActions;

	std::string m_driverInfo;
	int m_viewportX;
	int m_viewportY;
	int m_viewportWidth;
	int m_viewportHeight;

	// current program
	Program* m_curProgram;

	// rasterizer state
	TriangleFace m_curTriangleFaceToCull;
	FillMode m_curFillMode;

	// sampler state
	TextureState m_textureStates[8];

	// blend state
	BlendFactor m_curSourceFactor;
	BlendFactor m_curDestinationFactor;

	// depth state
	bool m_curDepthMask;
	bool m_renderToTexture;
	CompareMode m_curDepthCompareMode;

	// stencil state
	int m_curStencilRef;
	CompareMode m_curStencilFunc;
	unsigned int m_curStencilMask;
	StencilAction m_curStencilBothPassOp;
	StencilAction m_curStencilDepthFailOp;
	StencilAction m_curStencilDepthPassStencilFailOp;
};

AWAY_NAMESPACE_END