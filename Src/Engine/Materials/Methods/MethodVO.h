#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

/**
 * MethodVO contains data for a given method for the use within a single material.
 * This allows methods to be shared across materials while their non-public state differs.
 */
class MethodVO
{
public:
	MethodVO();
	void reset();

public:
	std::vector<float>* m_vertexData;
	std::vector<float>* m_fragmentData;

	int m_texturesIndex;
	int m_secondaryTexturesIndex;
	int m_vertexConstantsIndex;
	int m_secondaryVertexConstantsIndex;
	int m_fragmentConstantsIndex;
	int m_secondaryFragmentConstantsIndex;

	bool m_useMipmapping;
	bool m_useSmoothTextures;
	bool m_repeatTextures;

	bool m_needsProjection;
	bool m_needsView;
	bool m_needsNormals;
	bool m_needsTangents;
	bool m_needsUV;
	bool m_needsSecondaryUV;
	bool m_needsGlobalVertexPos;
	bool m_needsGlobalFragmentPos;

	bool m_useLightFallOff;
	int m_numLights;
};

AWAY_NAMESPACE_END