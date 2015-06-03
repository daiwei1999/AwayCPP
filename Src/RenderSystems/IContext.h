#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

enum class BlendFactor
{
	DESTINATION_ALPHA,
	DESTINATION_COLOR,
	ONE,
	ONE_MINUS_DESTINATION_ALPHA,
	ONE_MINUS_DESTINATION_COLOR,
	ONE_MINUS_SOURCE_ALPHA,
	ONE_MINUS_SOURCE_COLOR,
	SOURCE_ALPHA,
	SOURCE_COLOR,
	ZERO
};

enum class BufferUsage
{
	DYNAMIC_DRAW,
	STATIC_DRAW
};

enum class ClearMask
{
	COLOR = 1,
	DEPTH = 2,
	STENCIL = 4,
	ALL = 7
};

enum class TriangleFace
{
	BACK,
	FRONT,
	FRONT_AND_BACK,
	NONE
};

enum class CompareMode
{
	ALWAYS,
	EQUAL,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL,
	NEVER,
	NOT_EQUAL
};

enum class FillMode
{
	SOLID,
	WIREFRAME
};

enum class ProgramType
{
	VERTEX,
	FRAGMENT
};

enum class StencilAction
{
	DECREMENT_SATURATE,
	DECREMENT_WRAP,
	INCREMENT_SATURATE,
	INCREMENT_WRAP,
	INVERT,
	KEEP,
	SET,
	ZERO
};

enum class TextureFormat
{
	BGRA,
	BGRA_PACKED, // 16 bit, bgra packed as 4:4:4:4
	BGR_PACKED, // 16 bit, bgr packed as 5:6:5
	COMPRESSED,
	COMPRESSED_ALPHA,
	RGBA_HALF_FLOAT
};

enum class WrapMode
{
	CLAMP,
	REPEAT
};

enum class TextureFilter
{
	NEAREST,
	LINEAR,
	ANISOTROPIC2X,
	ANISOTROPIC4X,
	ANISOTROPIC8X,
	ANISOTROPIC16X,
};

enum class MipFilter
{
	MIPLINEAR,
	MIPNEAREST,
	MIPNONE
};

enum class VertexBufferFormat
{
	BYTES_1,
	BYTES_2,
	BYTES_3,
	BYTES_4,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,
	FLOAT_4
};

class IContext
{
public:
	virtual ~IContext() {}
	virtual const std::string& driverInfo() = 0;

	virtual void clear(float red = 0, float green = 0, float blue = 0, float alpha = 1, float depth = 1, unsigned int stencil = 0, ClearMask mask = ClearMask::ALL) = 0;
	virtual void configureViewport(int x, int y, int with, int height, bool enableStencil = true) = 0;

	virtual Texture* createTexture(int width, int height, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels = 0) = 0;
	virtual CubeTexture* createCubeTexture(int size, TextureFormat format, bool optimizeForRenderToTexture, int streamingLevels = 0) = 0;
	
	virtual IndexBuffer* createIndexBuffer(int numIndices, BufferUsage bufferUsage = BufferUsage::STATIC_DRAW) = 0;
	virtual VertexBuffer* createVertexBuffer(int numVertices, int bytesPerVertex, BufferUsage bufferUsage = BufferUsage::STATIC_DRAW) = 0;

	virtual Program* createProgram() = 0;

	virtual void drawLines(IndexBuffer* indexBuffer, int firstIndex, int numLines) = 0;
	virtual void drawTriangles(IndexBuffer* indexBuffer, int firstIndex, int numTriangles) = 0;
	virtual void present() = 0;

	virtual void setBlendFactors(BlendFactor sourceFactor, BlendFactor destinationFactor) = 0;
	virtual void setColorMask(bool red, bool green, bool blue, bool alpha) = 0;
	virtual void setCulling(TriangleFace triangleFaceToCull) = 0;
	virtual void setDepthTest(bool depthMask, CompareMode passCompareMode) = 0;
	virtual void setFillMode(FillMode fillMode) = 0;
	virtual void setProgram(Program* program) = 0;

	virtual void setProgramConstantsFromMatrix(ProgramType programType, int firstRegister, Matrix3D& matrix, bool transposedMatrix = false) = 0;
	virtual void setProgramConstantsFromVector(ProgramType programType, int firstRegister, float* data, int numRegisters) = 0;

	virtual void setRenderToBackBuffer() = 0;
	virtual void setRenderToTexture(TextureBase* texture, bool enableDepthAndStencil = false, int antiAlias = 0, int surfaceSelector = 0, int colorOutputIndex = 0) = 0;
	virtual void setSamplerStateAt(int sampler, WrapMode wrap, TextureFilter filter, MipFilter mipfilter) = 0;
	virtual void setScissorRectangle(Rectangle* rectangle) = 0;
	virtual void setStencilActions(TriangleFace triangleFace = TriangleFace::FRONT_AND_BACK,
		                           CompareMode compareMode = CompareMode::ALWAYS,
								   StencilAction actionOnBothPass = StencilAction::KEEP,
								   StencilAction actionOnDepthFail = StencilAction::KEEP,
								   StencilAction actionOnDepthPassStencilFail = StencilAction::KEEP) = 0;
	virtual void setStencilReferenceValue(int referenceValue, unsigned int readMask, unsigned int writeMask) = 0;
	virtual void setTextureAt(int sampler, TextureBase* texture) = 0;
	virtual void setVertexBufferAt(int index, VertexBuffer* buffer, int offset = 0, VertexBufferFormat format = VertexBufferFormat::FLOAT_4) = 0;
};

AWAY_NAMESPACE_END