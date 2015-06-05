#pragma once

#include "Common.h"
#include "Entity.h"
#include "IMaterialOwner.h"

AWAY_NAMESPACE_BEGIN

/**
 * Mesh is an instance of a Geometry, augmenting it with a presence in the scene graph, a material, and an animation
 * state. It consists out of SubMeshes, which in turn correspond to SubGeometries. SubMeshes allow different parts
 * of the geometry to be assigned different materials.
 */
class Mesh : public Entity, public IMaterialOwner
{
	DEFINE_CALLBACK_CLASS(Mesh, onSubGeometryAdded)
	DEFINE_CALLBACK_CLASS(Mesh, onSubGeometryRemoved)
	DEFINE_CALLBACK_CLASS(Mesh, onGeometryBoundsInvalid)

public:
	Mesh(Geometry* geometry, MaterialBase* material = nullptr);
	~Mesh();

	bool getCastsShadows() { return m_castsShadows; }
	void setCastsShadows(bool value) { m_castsShadows = value; }

	IAnimator* getAnimator() override { return m_animator; }
	void setAnimator(IAnimator* value);

	Geometry* getGeometry() { return m_geometry; }
	void setGeometry(Geometry* value);

	MaterialBase* getMaterial() override { return m_material; }
	void setMaterial(MaterialBase* value);

	SubMeshVector& getSubMeshes() { return m_subMeshes; }
	bool getShareAnimationGeometry() { return m_shareAnimationGeometry; }
	void setShareAnimationGeometry(bool value) { m_shareAnimationGeometry = value; }
	bool collidesBefore(float shortestCollisionDistance, bool findClosest) override;

	void addJointTag(JointTag* jointTag);
	int getJointTagCount() { return m_jointTags.size(); }
	JointTag* getJointTagAt(int index) { return m_jointTags[index]; }
	JointTag* getJointTagByName(const std::string& name);

protected:
	void updateBounds() override;
	EntityNode* createEntityPartitionNode() override;

private:
	void onGeometryBoundsInvalid(Event* event);
	void onSubGeometryAdded(Event* event);
	void onSubGeometryRemoved(Event* event);
	void addSubMesh(ISubGeometry* subGeometry);

protected:
	Geometry* m_geometry;

private:
	SubMeshVector m_subMeshes;
	MaterialBase* m_material;
	IAnimator* m_animator;
	bool m_castsShadows;
	bool m_shareAnimationGeometry;
	std::vector<JointTag*> m_jointTags;
};

AWAY_NAMESPACE_END