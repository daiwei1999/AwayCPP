#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ISubGeometry
{
public:
	/**
	 * The total amount of vertices in the SubGeometry.
	 */
	virtual int getVertexCount() = 0;

	/**
	 * The amount of triangles that comprise the IRenderable geometry.
	 */
	virtual int getTriangleCount() = 0;

	/**
	 * The distance between two consecutive vertex, normal or tangent elements
	 * This always applies to vertices, normals and tangents.
	 */
	virtual int getVertexStride() = 0;

	/**
	 * The distance between two consecutive normal elements
	 * This always applies to vertices, normals and tangents.
	 */
	virtual int getVertexNormalStride() = 0;

	/**
	 * The distance between two consecutive tangent elements
	 * This always applies to vertices, normals and tangents.
	 */
	virtual int getVertexTangentStride() = 0;

	/**
	 * The distance between two consecutive UV elements
	 */
	virtual int getUVStride() = 0;

	/**
	 * The distance between two secondary UV elements
	 */
	virtual int getSecondaryUVStride() = 0;

	/**
	 * Assigns the attribute stream for vertex positions.
	 * @param index The attribute stream index for the vertex shader
	 * @param stage3DProxy The Stage3DProxy to assign the stream to
	 */
	virtual void activateVertexBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for UV coordinates
	 * @param index The attribute stream index for the vertex shader
	 * @param stage3DProxy The Stage3DProxy to assign the stream to
	 */
	virtual void activateUVBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for a secondary set of UV coordinates
	 * @param index The attribute stream index for the vertex shader
	 * @param stage3DProxy The Stage3DProxy to assign the stream to
	 */
	virtual void activateSecondaryUVBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for vertex normals
	 * @param index The attribute stream index for the vertex shader
	 * @param stage3DProxy The Stage3DProxy to assign the stream to
	 */
	virtual void activateVertexNormalBuffer(int index, IContext* context) = 0;

	/**
	 * Assigns the attribute stream for vertex tangents
	 * @param index The attribute stream index for the vertex shader
	 * @param stage3DProxy The Stage3DProxy to assign the stream to
	 */
	virtual void activateVertexTangentBuffer(int index, IContext* context) = 0;

	/**
	 * Retrieves the IndexBuffer3D object that contains triangle indices.
	 * @param context The Context3D for which we request the buffer
	 * @return The VertexBuffer3D object that contains triangle indices.
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
	* Retrieves the object's uvs as a Number array.
	*/
	virtual float* getUVData() = 0;

	/**
	* Retrieves the object's secondary uvs as a Number array.
	*/
	virtual float* getSecondaryUVData() = 0;

	/**
	 * The offset into vertexData where the vertices are placed
	 */
	virtual int getVertexOffset() = 0;

	/**
	 * The offset into vertexNormalData where the normals are placed
	 */
	virtual int getVertexNormalOffset() = 0;

	/**
	 * The offset into vertexTangentData where the tangents are placed
	 */
	virtual int getVertexTangentOffset() = 0;

	/**
	 * The offset into UVData vector where the UVs are placed
	 */
	virtual int getUVOffset() = 0;

	/**
	 * The offset into SecondaryUVData vector where the UVs are placed
	 */
	virtual int getSecondaryUVOffset() = 0;

	/**
	 * Retrieves the object's indices as a uint array.
	 */
	virtual unsigned short* getIndexData() = 0;

	/**
	 * Retrieves the owner geometry of this SubGeometry.
	 */
	virtual Geometry* getParentGeometry() = 0;

	/**
	 * Sets the owner geometry of this SubGeometry.
	 */
	virtual void setParentGeometry(Geometry* value) = 0;
};

AWAY_NAMESPACE_END