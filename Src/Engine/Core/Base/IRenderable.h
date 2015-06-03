#pragma once

#include "Common.h"
#include "IMaterialOwner.h"

AWAY_NAMESPACE_BEGIN

/**
 * IRenderable provides an interface for objects that can be rendered in the rendering pipeline.
 */
class IRenderable : public IMaterialOwner
{
public:
	/**
	 * The transformation matrix that transforms from model to world space, adapted with any special operations needed to render.
	 * For example, assuring certain alignedness which is not inherent in the scene transform. By default, this would
	 * return the scene transform.
	 */
	virtual Matrix3D& getRenderSceneTransform(Camera3D* camera) = 0;

	/**
	 * The inverse scene transform object that transforms from world to model space.
	 */
	virtual Matrix3D& getInverseSceneTransform() = 0;

	/**
	 * Indicates whether the IRenderable should trigger mouse events, and hence should be rendered for hit testing.
	 */
	virtual bool getMouseEnabled() = 0;

	/**
	 * The entity that that initially provided the IRenderable to the render pipeline.
	 */
	virtual Entity* getSourceEntity() = 0;

	/**
	 * Indicates whether the renderable can cast shadows
	 */
	virtual bool getCastsShadows() = 0;

	/**
	 * Provides a Matrix object to transform the uv coordinates, if the material supports it.
	 * For TextureMaterial and TextureMultiPassMaterial, the animateUVs property should be set to true.
	 */
	virtual Matrix* getUVTransform() = 0;

	/**
	 * The amount of triangles that comprise the IRenderable geometry.
	 */
	virtual int getTriangleCount() = 0;

	/**
	 * The number of data elements in the buffers per vertex.
	 * This always applies to vertices, normals and tangents.
	 */
	virtual int getVertexStride() = 0;

	/**
	 * Assigns the attribute stream for vertex positions.
	 * @param index The attribute stream index for the vertex shader
	 * @param context The Context to assign the stream to
	 */
	virtual void activateVertexBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for UV coordinates
	 * @param index The attribute stream index for the vertex shader
	 * @param context The Context to assign the stream to
	 */
	virtual void activateUVBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for a secondary set of UV coordinates
	 * @param index The attribute stream index for the vertex shader
	 * @param context The Context to assign the stream to
	 */
	virtual void activateSecondaryUVBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for vertex normals
	 * @param index The attribute stream index for the vertex shader
	 * @param context The Context to assign the stream to
	 */
	virtual void activateVertexNormalBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for vertex tangents
	 * @param index The attribute stream index for the vertex shader
	 * @param context The Context to assign the stream to
	 */
	virtual void activateVertexTangentBuffer(int index, IContext* context) = 0;

	/**
	 * Retrieves the IndexBuffer object that contains triangle indices.
	 * @param context The Context for which we request the buffer
	 * @return The VertexBuffer object that contains triangle indices.
	 */
	virtual IndexBuffer* getIndexBuffer(IContext* context) = 0;

	/**
	 * Retrieves the object's vertices as a Number array.
	 */
	virtual float* getVertexData() = 0;

	/**
	 * Retrieves the object's normals as a Number array.
	 */
	virtual float* getVertexNormalData() = 0;

	/**
	 * Retrieves the object's tangents as a Number array.
	 */
	virtual float* getVertexTangentData() = 0;

	/**
	 * Retrieves the object's indices as a uint array.
	 */
	virtual unsigned short* getIndexData() = 0;

	/**
	 * Retrieves the object's uvs as a Number array.
	 */
	virtual float* getUVData() = 0;
};

AWAY_NAMESPACE_END