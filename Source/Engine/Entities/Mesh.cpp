#include "Mesh.h"
#include "AxisAlignedBoundingBox.h"
#include "SubMesh.h"
#include "MeshNode.h"
#include "Geometry.h"
#include "GeometryEvent.h"
#include "MaterialBase.h"
#include "IAnimator.h"
#include "JointTag.h"

USING_AWAY_NAMESPACE

Mesh::Mesh(Geometry* geometry, MaterialBase* material)
{
	m_bounds = new AxisAlignedBoundingBox();
	m_worldBounds = new AxisAlignedBoundingBox();
	m_geometry = nullptr;
	m_material = nullptr;
	m_animator = nullptr;
	m_castsShadows = false;
	m_shareAnimationGeometry = true;
	setGeometry(geometry);
	setMaterial(material);
}

Mesh::~Mesh()
{

}

void Mesh::setAnimator(IAnimator* value)
{
	if (m_animator)
		m_animator->removeOwner(this);

	m_animator = value;

	// cause material to be unregistered and registered again to work with the new animation type (if possible)
	MaterialBase* oldMaterial = m_material;
	setMaterial(nullptr);
	setMaterial(oldMaterial);

	// reassign for each SubMesh
	for (SubMesh* subMesh : m_subMeshes)
	{
		oldMaterial = subMesh->getMaterial();
		if (oldMaterial)
		{
			subMesh->setMaterial(nullptr);
			subMesh->setMaterial(oldMaterial);
		}
	}

	if (m_animator)
		m_animator->addOwner(this);
}

void Mesh::setGeometry(Geometry* value)
{
	if (m_geometry)
	{
		m_geometry->removeEventListener(GeometryEvent::SUB_GEOMETRY_ADDED, CALLBACK_OBJECT(onSubGeometryAdded));
		m_geometry->removeEventListener(GeometryEvent::SUB_GEOMETRY_REMOVED, CALLBACK_OBJECT(onSubGeometryRemoved));
		m_geometry->removeEventListener(GeometryEvent::BOUNDS_INVALID, CALLBACK_OBJECT(onGeometryBoundsInvalid));

		for (auto subMesh : m_subMeshes)
			delete subMesh;
		
		m_subMeshes.clear();
	}

	m_geometry = value;

	if (m_geometry)
	{
		m_geometry->addEventListener(GeometryEvent::SUB_GEOMETRY_ADDED, CALLBACK_OBJECT(onSubGeometryAdded));
		m_geometry->addEventListener(GeometryEvent::SUB_GEOMETRY_REMOVED, CALLBACK_OBJECT(onSubGeometryRemoved));
		m_geometry->addEventListener(GeometryEvent::BOUNDS_INVALID, CALLBACK_OBJECT(onGeometryBoundsInvalid));

		for (auto subGeom : m_geometry->getSubGeometries())
			addSubMesh(subGeom);
	}
}

void Mesh::setMaterial(MaterialBase* value)
{
	if (value == m_material)
		return;

	if (m_material)
		m_material->removeOwner(this);

	m_material = value;

	if (m_material)
		m_material->addOwner(this);
}

bool Mesh::collidesBefore(float shortestCollisionDistance, bool findClosest)
{
	return true;
}

void Mesh::addJointTag(JointTag* jointTag)
{
	addChild(jointTag);
	m_jointTags.push_back(jointTag);
}

JointTag* Mesh::getJointTagByName(const std::string& name)
{
	for (auto jointTag : m_jointTags)
	{
		if (jointTag->getName() == name)
			return jointTag;
	}
	return nullptr;
}

void Mesh::updateBounds()
{
	m_bounds->fromGeometry(m_geometry);
	m_boundsInvalid = false;
}

EntityNode* Mesh::createEntityPartitionNode()
{
	return new MeshNode(this);
}

void Mesh::onGeometryBoundsInvalid(Event* event)
{
	invalidateBounds();
}

void Mesh::onSubGeometryAdded(Event* event)
{
	addSubMesh(((GeometryEvent*)event)->getSubGeometry());
}

void Mesh::onSubGeometryRemoved(Event* event)
{
	ISubGeometry* subGeom = ((GeometryEvent*)event)->getSubGeometry();
	for (SubMeshVector::iterator iter = m_subMeshes.begin(); iter != m_subMeshes.end(); iter++)
	{
		SubMesh* subMesh = *iter;
		if (subMesh->getSubGeometry() == subGeom)
		{
			delete subMesh;
			m_subMeshes.erase(iter);
			break;
		}
	}
}

void Mesh::addSubMesh(ISubGeometry* subGeometry)
{
	m_subMeshes.push_back(new SubMesh(subGeometry, this, nullptr));
	invalidateBounds();
}