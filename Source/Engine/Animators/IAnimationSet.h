#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * Provides an interface for data set classes that hold animation data for use in animator classes.
 */
class IAnimationSet
{
public:
	/**
	 * Check to determine whether a state is registered in the animation set under the given name.
	 *
	 * @param name The name of the animation state object to be checked.
	 */
	virtual bool hasAnimation(const std::string& name) = 0;

	/**
	 * Retrieves the animation state object registered in the animation data set under the given name.
	 *
	 * @param stateName The name of the animation state object to be retrieved.
	 */
	virtual AnimationNodeBase* getAnimation(const std::string& name) = 0;

	/**
	 * Indicates whether the properties of the animation data contained within the set combined with
	 * the vertex registers already in use on shading materials allows the animation data to utilize
	 * GPU calls.
	 */
	virtual bool usesCPU() = 0;

	/**
	 * Called by the material to reset the GPU indicator before testing whether register space in the shader
	 * is available for running GPU-based animation code.
	 */
	virtual void resetGPUCompatibility() = 0;

	/**
	 * Called by the animator to void the GPU indicator when register space in the shader
	 * is no longer available for running GPU-based animation code.
	 */
	virtual void cancelGPUCompatibility() = 0;

	/**
	 * Generates the AGAL Vertex code for the animation, tailored to the material pass's requirements.
	 *
	 * @param pass The MaterialPassBase object to whose vertex code the animation's code will be prepended.
	 * @sourceRegisters The animatable attribute registers of the material pass.
	 * @targetRegisters The animatable target registers of the material pass.
	 * @return The AGAL Vertex code that animates the vertex data.
	 */
	virtual void getAGALVertexCode(ShaderChunk& code, MaterialPassBase* pass, std::vector<unsigned int>& sourceRegisters, std::vector<unsigned int>& targetRegisters) = 0;

	/**
	 * Generates the AGAL Fragment code for the animation, tailored to the material pass's requirements.
	 *
	 * @param pass The MaterialPassBase object to whose vertex code the animation's code will be prepended.
	 * @return The AGAL Vertex code that animates the vertex data.
	 */
	virtual void getAGALFragmentCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int shadedTarget) = 0;

	/**
	 * Generates the extra AGAL Fragment code for the animation when UVs are required, tailored to the material pass's requirements.
	 *
	 * @param pass The MaterialPassBase object to whose vertex code the animation's code will be prepended.
	 * @param UVSource String representing the UV source register.
	 * @param UVTarget String representing the UV target register.
	 * @return The AGAL UV code that animates the UV data.
	*/
	virtual void getAGALUVCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int uvSource, unsigned int uvTarget) = 0;

	/**
	 * Resets any constants used in the creation of AGAL for the vertex and fragment shaders.
	 *
	 * @param pass The material pass currently being used to render the geometry.
	 */
	virtual void doneAGALCode(MaterialPassBase* pass) = 0;

	/**
	 * Sets the GPU render state required by the animation that is independent of the rendered mesh.
	 *
	 * @param stage3DProxy The proxy currently performing the rendering.
	 * @param pass The material pass currently being used to render the geometry.
	 */
	virtual void activate(IContext* context, MaterialPassBase* pass) = 0;

	/**
	 * Clears the GPU render state that has been set by the current animation.
	 *
	 * @param stage3DProxy The proxy currently performing the rendering.
	 * @param pass The material pass currently being used to render the geometry.
	 */
	virtual void deactivate(IContext* context, MaterialPassBase* pass) = 0;
};

AWAY_NAMESPACE_END