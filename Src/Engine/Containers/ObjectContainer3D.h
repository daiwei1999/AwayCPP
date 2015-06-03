#pragma once

#include "Common.h"
#include "Object3D.h"

AWAY_NAMESPACE_BEGIN

/**
 * ObjectContainer3D is the most basic scene graph node. It can contain other ObjectContainer3Ds.
 *
 * ObjectContainer3D can have its own scene partition assigned. However, when assigned to a different scene,
 * it will loose any partition information, since partitions are tied to a scene.
 */
class ObjectContainer3D : public Object3D
{
public:
	ObjectContainer3D();
	~ObjectContainer3D() {}

	ObjectContainer3D* addChild(ObjectContainer3D* child); // Adds a child ObjectContainer3D to the current object. The child's transformation will become relative to the current object's transformation.
	void removeChild(ObjectContainer3D* child);
	void removeChildAt(int index) { removeChildInternal(index, m_children[index]); }
	ObjectContainer3D* getChildAt(int index) { return m_children[index]; }
	int getChildCount() { return m_children.size(); }
	bool contains(ObjectContainer3D* child) { return std::find(m_children.begin(), m_children.end(), child) != m_children.end(); }
	ObjectContainer3D* getParent() { return m_parent; }
	void removeFromParent();

	Scene3D* getScene() { return m_scene; }
	virtual void setScene(Scene3D* value);

	Partition3D* getPartition() { return m_explicitPartition; }
	void setPartition(Partition3D* value);

	Matrix3D& getSceneTransform();
	Matrix3D& getInverseSceneTransform();

	bool isVisible() { return m_implicitVisibility && m_explicitVisibility; }
	bool getMouseEnabled() { return m_mouseEnabled;	}
	void setMouseEnabled(bool value);
	bool getMouseChildren() { return m_mouseChildren; }
	void setMouseChildren(bool value);
	bool getVisible() { return m_explicitVisibility; }
	void setVisible(bool value);

	Vector3D* getScenePosition();
	virtual float getMinX();
	virtual float getMinY();
	virtual float getMinZ();
	virtual float getMaxX();
	virtual float getMaxY();
	virtual float getMaxZ();
	void translateLocal(Vector3D& axis, float distance) override;
	void rotate(Vector3D& axis, float angle) override;
	void lookAt(Vector3D& target, Vector3D& upAxis = Vector3D::Y_AXIS) override;
	void invalidateTransform() override;

	Partition3D* getImplicitPartition() { return m_implicitPartition; }
	virtual void setImplicitPartition(Partition3D* value);
	void updateImplicitVisibility();

protected:
	virtual void invalidateSceneTransform();
	virtual void updateSceneTransform();
	virtual void updateMouseChildren();

private:
	void setParent(ObjectContainer3D* value);
	void notifySceneTransformChange();
	void removeChildInternal(int index, ObjectContainer3D* child);

public:
	bool m_isRoot;
	bool m_ancestorsAllowMouseEnabled;

protected:
	Scene3D* m_scene;
	ObjectContainer3D* m_parent;
	Matrix3D m_sceneTransform;
	Partition3D* m_explicitPartition;
	Partition3D* m_implicitPartition;
	bool m_sceneTransformDirty;
	bool m_ignoreTransform;
	bool m_mouseEnabled;

private:
	std::vector<ObjectContainer3D*> m_children;
	Scene3D* m_oldScene;
	Matrix3D m_inverseSceneTransform;
	Vector3D m_scenePosition;
	bool m_inverseSceneTransformDirty;
	bool m_scenePositionDirty;
	bool m_explicitVisibility;
	bool m_implicitVisibility;
	bool m_mouseChildren;
};

AWAY_NAMESPACE_END