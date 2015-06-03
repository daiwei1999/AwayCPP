#include "ObjectContainer3D.h"

USING_AWAY_NAMESPACE

ObjectContainer3D::ObjectContainer3D()
{
	m_scene = m_oldScene = nullptr;
	m_parent = nullptr;
	m_explicitPartition = m_implicitPartition = nullptr;
	m_sceneTransformDirty = true;
	m_ignoreTransform = false;
	m_mouseEnabled = false;
	m_inverseSceneTransformDirty = true;
	m_scenePositionDirty = true;
	m_explicitVisibility = true;
	m_implicitVisibility = true;
	m_mouseChildren = true;
	m_isRoot = false;
}

ObjectContainer3D* ObjectContainer3D::addChild(ObjectContainer3D* child)
{
	if (child)
	{
		if (child->m_parent)
			child->m_parent->removeChild(child);

		if (!child->m_explicitPartition)
			child->setImplicitPartition(m_implicitPartition);

		child->setParent(this);
		child->setScene(m_scene);
		child->notifySceneTransformChange();
		child->updateMouseChildren();
		child->updateImplicitVisibility();

		m_children.push_back(child);
	}
	return child;
}

void ObjectContainer3D::removeChild(ObjectContainer3D* child)
{
	auto iter = std::find(m_children.begin(), m_children.end(), child);
	if (iter != m_children.end())
		removeChildInternal(iter - m_children.begin(), child);
}

void ObjectContainer3D::removeFromParent()
{
	if (m_parent)
		m_parent->removeChild(this);
}

void ObjectContainer3D::setScene(Scene3D* value)
{
	for (auto child : m_children)
		child->setScene(value);

	if (m_scene == value)
		return;

	// test to see if we're switching roots while we're already using a scene partition
	if (value == nullptr)
		m_oldScene = m_scene;

	if (m_explicitPartition && m_oldScene && m_oldScene != m_scene)
		setPartition(nullptr);

	if (value)
		m_oldScene = nullptr;

	m_scene = value;
}

void ObjectContainer3D::setPartition(Partition3D* value)
{
	m_explicitPartition = value;
	setImplicitPartition(value ? value : (m_parent ? m_parent->getImplicitPartition() : nullptr));
}

Matrix3D& ObjectContainer3D::getSceneTransform()
{
	if (m_sceneTransformDirty)
		updateSceneTransform();

	return m_sceneTransform;
}

void ObjectContainer3D::setMouseChildren(bool value)
{
	m_mouseChildren = value;
	updateMouseChildren();
}

Matrix3D& ObjectContainer3D::getInverseSceneTransform()
{
	if (m_inverseSceneTransformDirty)
	{
		m_inverseSceneTransform.copyFrom(getSceneTransform());
		m_inverseSceneTransform.invert();
		m_inverseSceneTransformDirty = false;
	}
	return m_inverseSceneTransform;
}

void ObjectContainer3D::setMouseEnabled(bool value)
{
	m_mouseEnabled = value;
	updateMouseChildren();
}

void ObjectContainer3D::setVisible(bool value)
{
	m_explicitVisibility = value;

	for (auto child : m_children)
		child->updateImplicitVisibility();
}

Vector3D* ObjectContainer3D::getScenePosition()
{
	if (m_scenePositionDirty)
	{
		getSceneTransform().copyColumnTo(3, m_scenePosition);
		m_scenePositionDirty = false;
	}
	return &m_scenePosition;
}

float ObjectContainer3D::getMinX()
{
	float m, min = MathConsts::Infinity;
	for (auto child : m_children)
	{
		m = child->getMinX() + child->getX();
		if (m < min)
			min = m;
	}
	return min;
}

float ObjectContainer3D::getMinY()
{
	float m, min = MathConsts::Infinity;
	for (auto child : m_children)
	{
		m = child->getMinY() + child->getY();
		if (m < min)
			min = m;
	}
	return min;
}

float ObjectContainer3D::getMinZ()
{
	float m, min = MathConsts::Infinity;
	for (auto child : m_children)
	{
		m = child->getMinZ() + child->getZ();
		if (m < min)
			min = m;
	}
	return min;
}

float ObjectContainer3D::getMaxX()
{
	float m, max = -MathConsts::Infinity;
	for (auto child : m_children)
	{
		m = child->getMaxX() + child->getX();
		if (m > max)
			max = m;
	}
	return max;
}

float ObjectContainer3D::getMaxY()
{
	float m, max = -MathConsts::Infinity;
	for (auto child : m_children)
	{
		m = child->getMaxY() + child->getY();
		if (m > max)
			max = m;
	}
	return max;
}

float ObjectContainer3D::getMaxZ()
{
	float m, max = -MathConsts::Infinity;
	for (auto child : m_children)
	{
		m = child->getMaxZ() + child->getZ();
		if (m > max)
			max = m;
	}
	return max;
}

void ObjectContainer3D::translateLocal(Vector3D& axis, float distance)
{
	Object3D::translateLocal(axis, distance);
	notifySceneTransformChange();
}

void ObjectContainer3D::rotate(Vector3D& axis, float angle)
{
	Object3D::rotate(axis, angle);
	notifySceneTransformChange();
}

void ObjectContainer3D::lookAt(Vector3D& target, Vector3D& upAxis)
{
	Object3D::lookAt(target, upAxis);
	notifySceneTransformChange();
}

void ObjectContainer3D::updateImplicitVisibility()
{
	m_implicitVisibility = m_parent->m_explicitVisibility && m_parent->m_implicitVisibility;

	for (auto child : m_children)
		child->updateImplicitVisibility();
}

void ObjectContainer3D::invalidateTransform()
{
	Object3D::invalidateTransform();
	notifySceneTransformChange();
}

void ObjectContainer3D::invalidateSceneTransform()
{
	m_sceneTransformDirty = !m_ignoreTransform;
	m_inverseSceneTransformDirty = !m_ignoreTransform;
	m_scenePositionDirty = !m_ignoreTransform;
}

void ObjectContainer3D::updateSceneTransform()
{
	if (m_parent && !m_parent->m_isRoot)
	{
		m_sceneTransform.copyFrom(m_parent->getSceneTransform());
		m_sceneTransform.prepend(getTransform());
	}
	else
	{
		m_sceneTransform.copyFrom(getTransform());
	}
	m_sceneTransformDirty = false;
}

void ObjectContainer3D::updateMouseChildren()
{
	if (m_parent && !m_parent->m_isRoot)
		m_ancestorsAllowMouseEnabled = m_parent->m_ancestorsAllowMouseEnabled && m_parent->m_mouseChildren;
	else
		m_ancestorsAllowMouseEnabled = m_mouseChildren;

	// Sweep children.
	for (auto child : m_children)
		child->updateMouseChildren();
}

void ObjectContainer3D::setImplicitPartition(Partition3D* value)
{
	if (value == m_implicitPartition)
		return;

	m_implicitPartition = value;

	for (auto child : m_children)
	{
		if (!child->m_explicitPartition)
			child->setImplicitPartition(value);
	}
}

void ObjectContainer3D::setParent(ObjectContainer3D* value)
{
	m_parent = value;
	updateMouseChildren();
	if (!value)
	{
		setScene(nullptr);
		return;
	}
	notifySceneTransformChange();
}

void ObjectContainer3D::notifySceneTransformChange()
{
	if (m_sceneTransformDirty || m_ignoreTransform)
		return;

	invalidateSceneTransform();

	for (auto child : m_children)
		child->notifySceneTransformChange();
}

void ObjectContainer3D::removeChildInternal(int index, ObjectContainer3D* child)
{
	// index is important because getChildAt needs to be regular.
	m_children.erase(m_children.begin() + index);

	// this needs to be nullified before the callbacks!
	child->setParent(nullptr);

	if (!child->m_explicitPartition)
		child->setImplicitPartition(nullptr);
}