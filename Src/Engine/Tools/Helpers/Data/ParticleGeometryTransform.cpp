#include "ParticleGeometryTransform.h"
#include "Matrix3D.h"

USING_AWAY_NAMESPACE

ParticleGeometryTransform::ParticleGeometryTransform()
{
	m_vertexTransform = nullptr;
	m_inverseVertexTransform = nullptr;
	m_uvTransform = nullptr;
}

void ParticleGeometryTransform::setVertexTransform(Matrix3D* value)
{
	m_vertexTransform = value;
	m_inverseVertexTransform = new Matrix3D(*value);
	m_inverseVertexTransform->invert();
	m_inverseVertexTransform->transpose();
}