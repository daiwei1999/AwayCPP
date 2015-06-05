#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ParticleGeometryTransform
{
public:
	ParticleGeometryTransform();

	void setVertexTransform(Matrix3D* value);
	Matrix3D* getVertexTransform() { return m_vertexTransform; }
	Matrix3D* getInverseVertexTransform() { return m_inverseVertexTransform; }
	void setUVTransform(Matrix* value) { m_uvTransform = value; }
	Matrix* getUVTransform() { return m_uvTransform; }

private:
	Matrix3D* m_vertexTransform;
	Matrix3D* m_inverseVertexTransform;
	Matrix* m_uvTransform;
};

AWAY_NAMESPACE_END