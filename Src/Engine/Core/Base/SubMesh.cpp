#include "SubMesh.h"
#include "MaterialBase.h"

USING_AWAY_NAMESPACE

SubMesh::SubMesh(ISubGeometry* subGeometry, Mesh* parentMesh, MaterialBase* material)
{
	m_animationSubGeometry = nullptr;
	m_animatorSubGeometry = nullptr;
	m_parentMesh = parentMesh;
	m_subGeometry = subGeometry;
	m_material = nullptr;
	m_uvRotation = 0;
	m_scaleU = 1;
	m_scaleV = 1;
	m_offsetU = 0;
	m_offsetV = 0;
	m_uvTransformDirty = false;
	setMaterial(material);
}

void SubMesh::setMaterial(MaterialBase* value)
{
	if (value == m_material)
		return;

	if (m_material)
		m_material->removeOwner(this);

	m_material = value;

	if (m_material)
		m_material->addOwner(this);
}

void SubMesh::setOffsetU(float value)
{
	if (value == m_offsetU)
		return;

	m_offsetU = value;
	m_uvTransformDirty = true;
}

void SubMesh::setOffsetV(float value)
{
	if (value == m_offsetV)
		return;

	m_offsetV = value;
	m_uvTransformDirty = true;
}

void SubMesh::setScaleU(float value)
{
	if (value == m_scaleU)
		return;

	m_scaleU = value;
	m_uvTransformDirty = true;
}

void SubMesh::setScaleV(float value)
{
	if (value == m_scaleV)
		return;

	m_scaleV = value;
	m_uvTransformDirty = true;
}

void SubMesh::setUVRotation(float value)
{
	if (value == m_uvRotation)
		return;

	m_uvRotation = value;
	m_uvTransformDirty = true;
}

Matrix* SubMesh::getUVTransform()
{
	if (m_uvTransformDirty)
		updateUVTransform();

	return &m_uvTransform;
}

void SubMesh::updateUVTransform()
{
	m_uvTransform.identity();
	if (m_uvRotation != 0)
		m_uvTransform.rotate(m_uvRotation);

	if (m_scaleU != 1 || m_scaleV != 1)
		m_uvTransform.scale(m_scaleU, m_scaleV);

	m_uvTransform.translate(m_offsetU, m_offsetV);
	m_uvTransformDirty = false;
}