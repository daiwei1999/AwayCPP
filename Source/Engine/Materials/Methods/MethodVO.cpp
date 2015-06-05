#include "MethodVO.h"

USING_AWAY_NAMESPACE

MethodVO::MethodVO()
{
	
}

void MethodVO::reset()
{
	m_texturesIndex = -1;
	m_vertexConstantsIndex = -1;
	m_fragmentConstantsIndex = -1;

	m_useMipmapping = true;
	m_useSmoothTextures = true;
	m_repeatTextures = false;

	m_needsProjection = false;
	m_needsView = false;
	m_needsNormals = false;
	m_needsTangents = false;
	m_needsUV = false;
	m_needsSecondaryUV = false;
	m_needsGlobalVertexPos = false;
	m_needsGlobalFragmentPos = false;

	m_numLights = 0;
	m_useLightFallOff = true;
}
