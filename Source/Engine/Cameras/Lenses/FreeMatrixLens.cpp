#include "FreeMatrixLens.h"
#include "PerspectiveLens.h"

USING_AWAY_NAMESPACE

FreeMatrixLens::FreeMatrixLens()
{
	PerspectiveLens lens;
	m_matrix.copyFrom(lens.getMatrix());
}

void FreeMatrixLens::setNear(float value)
{
	m_near = value;
}

void FreeMatrixLens::setFar(float value)
{
	m_far = value;
}

void FreeMatrixLens::setAspectRatio(float value)
{
	m_aspectRatio = value;
}

void FreeMatrixLens::updateMatrix()
{
	m_matrixInvalid = false;
}

Vector3D* FreeMatrixLens::unproject(float nX, float nY, float sZ, Vector3D* result)
{
	return nullptr;
}