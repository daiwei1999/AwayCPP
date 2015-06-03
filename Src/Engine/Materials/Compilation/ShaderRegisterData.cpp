#include "ShaderRegisterData.h"

USING_AWAY_NAMESPACE

ShaderRegisterData::ShaderRegisterData()
{
	m_normalVarying = 0;
	m_tangentVarying = 0;
	m_bitangentVarying = 0;
	m_uvVarying = 0;
	m_secondaryUVVarying = 0;
	m_viewDirVarying = 0;
	m_shadedTarget = 0;
	m_globalPositionVertex = 0;
	m_globalPositionVarying = 0;
	m_localPosition = 0;
	m_uvInput = 0;
	m_secondaryUVInput = 0;
	m_normalInput = 0;
	m_tangentInput = 0;
	m_animatedNormal = 0;
	m_animatedTangent = 0;
	m_commons = 0;
	m_projectionFragment = 0;
	m_normalFragment = 0;
	m_viewDirFragment = 0;
	m_bitangent = 0;
}