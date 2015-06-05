#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an interface for animator classes that control animation output from a data set subtype of <code>AnimationSetBase</code>.
 */
class IAnimator
{
public:
	/**
	 * Returns the animation data set in use by the animator.
	 */
	virtual IAnimationSet* getAnimationSet() = 0;

	/**
	 * Sets the GPU render state required by the animation that is dependent of the rendered object.
	 *
	 * @param stage3DProxy The Stage3DProxy object which is currently being used for rendering.
	 * @param renderable The object currently being rendered.
	 * @param vertexConstantOffset The first available vertex register to write data to if running on the gpu.
	 * @param vertexStreamOffset The first available vertex stream to write vertex data to if running on the gpu.
	 */
	virtual void setRenderState(IContext* context, IRenderable* renderable, int vertexConstantOffset, int vertexStreamOffset, Camera3D* camera) = 0;

	/**
	 * Verifies if the animation will be used on cpu. Needs to be true for all passes for a material to be able to use it on gpu.
	 * Needs to be called if gpu code is potentially required.
	 */
	virtual void testGPUCompatibility(MaterialPassBase* pass) = 0;

	/**
	 * Used by the mesh object to which the animator is applied, registers the owner for internal use.
	 */
	virtual void addOwner(Mesh* mesh) = 0;

	/**
	 * Used by the mesh object from which the animator is removed, unregisters the owner for internal use.
	 */
	virtual void removeOwner(Mesh* mesh) = 0;

	/**
	 * Returns a shallow clone (re-using the same IAnimationSet) of this IAnimator.
	 */
	virtual IAnimator* clone() = 0;
};

AWAY_NAMESPACE_END