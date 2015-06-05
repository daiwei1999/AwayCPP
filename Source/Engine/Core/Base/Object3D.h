#pragma once

#include "Common.h"
#include "Matrix3D.h"
#include "Vector3D.h"
#include "MathConsts.h"
#include "AssetBase.h"

AWAY_NAMESPACE_BEGIN

/**
 * Object3D provides a base class for any 3D object that has a (local) transformation.<br/><br/>
 *
 * Standard Transform:
 * <ul>
 *     <li> The standard order for transformation is [parent transform] * (Translate+Pivot) * (Rotate) * (-Pivot) * (Scale) * [child transform] </li>
 *     <li> This is the order of matrix multiplications, left-to-right. </li>
 *     <li> The order of transformation is right-to-left, however!
 *          (Scale) happens before (-Pivot) happens before (Rotate) happens before (Translate+Pivot)
 *          with no pivot, the above transform works out to [parent transform] * Translate * Rotate * Scale * [child transform]
 *          (Scale) happens before (Rotate) happens before (Translate) </li>
 *     <li> This is based on code in updateTransform and ObjectContainer3D.updateSceneTransform(). </li>
 *     <li> Matrix3D prepend = operator on rhs - e.g. transform' = transform * rhs; </li>
 *     <li> Matrix3D append =  operator on lhr - e.g. transform' = lhs * transform; </li>
 * </ul>
 *
 * To affect Scale:
 * <ul>
 *     <li> set scaleX/Y/Z directly, or call scale(delta) </li>
 * </ul>
 *
 * To affect Pivot:
 * <ul>
 *     <li> set pivotPoint directly, or call movePivot() </li>
 * </ul>
 *
 * To affect Rotate:
 * <ul>
 *    <li> set rotationX/Y/Z individually (using degrees), set eulers [all 3 angles] (using radians), or call rotateTo()</li>
 *    <li> call pitch()/yaw()/roll()/rotate() to add an additional rotation *before* the current transform.
 *         rotationX/Y/Z will be reset based on these operations. </li>
 * </ul>
 *
 * To affect Translate (post-rotate translate):
 *
 * <ul>
 *    <li> set x/y/z/position or call moveTo(). </li>
 *    <li> call translate(), which modifies x/y/z based on a delta vector. </li>
 *    <li> call moveForward()/moveBackward()/moveLeft()/moveRight()/moveUp()/moveDown()/translateLocal() to add an
 *         additional translate *before* the current transform. x/y/z will be reset based on these operations. </li>
 * </ul>
 */
class Object3D : public AssetBase
{
public:
	Object3D();
	virtual ~Object3D() {}

public:
	float getX() { return m_x; }
	void setX(float value);
	float getY() { return m_y; }
	void setY(float value);
	float getZ() { return m_z; }
	void setZ(float value);

	float getRotationX() { return m_rotationX * MathConsts::RADIANS_TO_DEGREES; }
	void setRotationX(float value);
	float getRotationY() { return m_rotationY * MathConsts::RADIANS_TO_DEGREES; }
	void setRotationY(float value);
	float getRotationZ() { return m_rotationZ * MathConsts::RADIANS_TO_DEGREES; }
	void setRotationZ(float value);

	float getScaleX() { return m_scaleX; }
	void setScaleX(float value);
	float getScaleY() { return m_scaleY; }
	void setScaleY(float value);
	float getScaleZ() { return m_scaleZ; }
	void setScaleZ(float value);

	void getEulers(Vector3D& result);
	void setEulers(Vector3D& value);
	Matrix3D& getTransform();
	void setTransform(Matrix3D& value);
	void getPosition(Vector3D& result);
	void setPosition(Vector3D* value);

	void getForwardVector(Vector3D& result);
	void getRightVector(Vector3D& result);
	void getUpVector(Vector3D& result);
	void getBackVector(Vector3D& result);
	void getLeftVector(Vector3D& result);
	void getDownVector(Vector3D& result);

	void scale(float value); // Appends a uniform scale to the current transformation
	void moveForward(float distance); // Moves the 3d object forwards along it's local z axis
	void moveBackward(float distance); // Moves the 3d object backwards along it's local z axis
	void moveLeft(float distance); // Moves the 3d object backwards along it's local x axis
	void moveRight(float distance); // Moves the 3d object forwards along it's local x axis
	void moveUp(float distance); // Moves the 3d object forwards along it's local y axis
	void moveDown(float distance); // Moves the 3d object backwards along it's local y axis
	void translate(Vector3D& axis, float distance); // Moves the 3d object along a vector by a defined length
	virtual void translateLocal(Vector3D& axis, float distance); // Moves the 3d object along a vector by a defined length
	void pitch(float angle); // Rotates the 3d object around it's local x-axis
	void yaw(float angle); // Rotates the 3d object around it's local y-axis
	void roll(float angle); // Rotates the 3d object around it's local z-axis
	virtual void rotate(Vector3D& axis, float angle); // Rotates the 3d object around an axis by a defined angle
	virtual void lookAt(Vector3D& target, Vector3D& upAxis = Vector3D::Y_AXIS); // Rotates the 3d object around to face a point defined relative to the local coordinates of the parent
	virtual void invalidateTransform() { m_transformDirty = true; }

protected:
	void updateTransform();

private:
	void invalidatePosition();
	void invalidateRotation();
	void invalidateScale();

protected:
	Matrix3D m_transform;
	float m_rotationX, m_rotationY, m_rotationZ; // Euler angles
	float m_scaleX, m_scaleY, m_scaleZ;
	float m_x, m_y, m_z;
	bool m_transformDirty, m_positionDirty, m_rotationDirty, m_scaleDirty;
};

AWAY_NAMESPACE_END