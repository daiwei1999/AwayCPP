#include "Object3D.h"

USING_AWAY_NAMESPACE

Object3D::Object3D()
{
	m_rotationX = m_rotationY = m_rotationZ = 0;
	m_scaleX = m_scaleY = m_scaleZ = 1;
	m_x = m_y = m_z = 0;
	m_transformDirty = true;
	m_positionDirty = m_rotationDirty = m_scaleDirty = false;
}

void Object3D::setX(float value)
{
	if (m_x != value)
	{
		m_x = value;
		invalidatePosition();
	}
}

void Object3D::setY(float value)
{
	if (m_y != value)
	{
		m_y = value;
		invalidatePosition();
	}
}

void Object3D::setZ(float value)
{
	if (m_z != value)
	{
		m_z = value;
		invalidatePosition();
	}
}

void Object3D::setRotationX(float value)
{
	if (getRotationX() != value)
	{
		m_rotationX = value * MathConsts::DEGREES_TO_RADIANS;
		invalidateRotation();
	}
}

void Object3D::setRotationY(float value)
{
	if (getRotationY() != value)
	{
		m_rotationY = value * MathConsts::DEGREES_TO_RADIANS;
		invalidateRotation();
	}
}

void Object3D::setRotationZ(float value)
{
	if (getRotationZ() != value)
	{
		m_rotationZ = value * MathConsts::DEGREES_TO_RADIANS;
		invalidateRotation();
	}
}

void Object3D::setScaleX(float value)
{
	if (m_scaleX != value)
	{
		m_scaleX = value;
		invalidateScale();
	}
}

void Object3D::setScaleY(float value)
{
	if (m_scaleY != value)
	{
		m_scaleY = value;
		invalidateScale();
	}
}

void Object3D::setScaleZ(float value)
{
	if (m_scaleZ != value)
	{
		m_scaleZ = value;
		invalidateScale();
	}
}

void Object3D::getEulers(Vector3D& result)
{
	result.m_x = m_rotationX * MathConsts::RADIANS_TO_DEGREES;
	result.m_y = m_rotationY * MathConsts::RADIANS_TO_DEGREES;
	result.m_z = m_rotationZ * MathConsts::RADIANS_TO_DEGREES;
}

void Object3D::setEulers(Vector3D& value)
{
	m_rotationX = value.m_x * MathConsts::DEGREES_TO_RADIANS;
	m_rotationY = value.m_y * MathConsts::DEGREES_TO_RADIANS;
	m_rotationZ = value.m_z * MathConsts::DEGREES_TO_RADIANS;
	invalidateRotation();
}

Matrix3D& Object3D::getTransform()
{
	if (m_transformDirty)
		updateTransform();

	return m_transform;
}

void Object3D::setTransform(Matrix3D& value)
{
	//ridiculous matrix error
	if (value.m_rawData[0] == 0)
		value.m_rawData[0] = 0.0000000000000000000001f;

	Vector3D elements[3];
	value.decompose(elements);

	Vector3D& vec = elements[0];
	if (m_x != vec.m_x || m_y != vec.m_y || m_z != vec.m_z)
	{
		m_x = vec.m_x;
		m_y = vec.m_y;
		m_z = vec.m_z;
		invalidatePosition();
	}

	vec = elements[1];
	if (m_rotationX != vec.m_x || m_rotationY != vec.m_y || m_rotationZ != vec.m_z)
	{
		m_rotationX = vec.m_x;
		m_rotationY = vec.m_y;
		m_rotationZ = vec.m_z;
		invalidateRotation();
	}

	vec = elements[2];
	if (m_scaleX != vec.m_x || m_scaleY != vec.m_y || m_scaleZ != vec.m_z)
	{
		m_scaleX = vec.m_x;
		m_scaleY = vec.m_y;
		m_scaleZ = vec.m_z;
		invalidateScale();
	}
}

void Object3D::getPosition(Vector3D& result)
{
	Matrix3D& transform = getTransform();
	transform.copyColumnTo(3, result);
}

void Object3D::setPosition(Vector3D* value)
{
	if (m_x != value->m_x || m_y != value->m_y || m_z != value->m_z)
	{
		m_x = value->m_x;
		m_y = value->m_y;
		m_z = value->m_z;
		invalidatePosition();
	}
}

void Object3D::getForwardVector(Vector3D& result)
{
	Matrix3D& transform = getTransform();
	transform.getForward(result);
}

void Object3D::getRightVector(Vector3D& result)
{
	Matrix3D& transform = getTransform();
	transform.getRight(result);
}

void Object3D::getUpVector(Vector3D& result)
{
	Matrix3D& transform = getTransform();
	transform.getUp(result);
}

void Object3D::getBackVector(Vector3D& result)
{
	getForwardVector(result);
	result.negate();
}

void Object3D::getLeftVector(Vector3D& result)
{
	getRightVector(result);
	result.negate();
}

void Object3D::getDownVector(Vector3D& result)
{
	getUpVector(result);
	result.negate();
}

void Object3D::scale(float value)
{
	m_scaleX *= value;
	m_scaleY *= value;
	m_scaleZ *= value;
	invalidateScale();
}

void Object3D::moveForward(float distance)
{
	translateLocal(Vector3D::Z_AXIS, distance);
}

void Object3D::moveBackward(float distance)
{
	translateLocal(Vector3D::Z_AXIS, -distance);
}

void Object3D::moveLeft(float distance)
{
	translateLocal(Vector3D::X_AXIS, -distance);
}

void Object3D::moveRight(float distance)
{
	translateLocal(Vector3D::X_AXIS, distance);
}

void Object3D::moveUp(float distance)
{
	translateLocal(Vector3D::Y_AXIS, distance);
}

void Object3D::moveDown(float distance)
{
	translateLocal(Vector3D::Y_AXIS, -distance);
}

void Object3D::translate(Vector3D& axis, float distance)
{
	float x = axis.m_x, y = axis.m_y, z = axis.m_z;
	float len = distance / std::sqrt(x * x + y * y + z * z);
	m_x += x * len;
	m_y += y * len;
	m_z += z * len;
	invalidatePosition();
}

void Object3D::translateLocal(Vector3D& axis, float distance)
{
	float x = axis.m_x, y = axis.m_y, z = axis.m_z;
	float len = distance / std::sqrt(x * x + y * y + z * z);

	Matrix3D& transform = getTransform();
	transform.prependTranslation(x * len, y * len, z * len);

	m_x = m_transform.m_rawData[12];
	m_y = m_transform.m_rawData[13];
	m_z = m_transform.m_rawData[14];
	invalidatePosition();
}

void Object3D::pitch(float angle)
{
	Vector3D axis;
	getRightVector(axis);
	rotate(axis, angle);
}

void Object3D::yaw(float angle)
{
	Vector3D axis;
	getUpVector(axis);
	rotate(axis, angle);
}

void Object3D::roll(float angle)
{
	Vector3D axis;
	getForwardVector(axis);
	rotate(axis, angle);
}

void Object3D::rotate(const Vector3D& axis, float angle)
{
	Vector3D pivotPoint;
	m_transform.copyColumnTo(3, pivotPoint);
	m_transform.appendRotation(angle, axis, pivotPoint);
	setTransform(m_transform);
}

void Object3D::lookAt(Vector3D& target, Vector3D& upAxis)
{
	if (m_transformDirty)
		updateTransform();

	Vector3D zAxis(target.m_x - m_x, target.m_y - m_y, target.m_z - m_z);
	zAxis.normalize();

	Vector3D xAxis(upAxis.m_y * zAxis.m_z - upAxis.m_z * zAxis.m_y, upAxis.m_z * zAxis.m_x - upAxis.m_x * zAxis.m_z, upAxis.m_x * zAxis.m_y - upAxis.m_y * zAxis.m_x);
	xAxis.normalize();

	if (xAxis.getLength() < .05f)
	{
		xAxis.m_x = upAxis.m_y;
		xAxis.m_y = upAxis.m_x;
		xAxis.m_z = 0;
		xAxis.normalize();
	}

	Vector3D yAxis(zAxis.m_y * xAxis.m_z - zAxis.m_z * xAxis.m_y, zAxis.m_z * xAxis.m_x - zAxis.m_x * xAxis.m_z, zAxis.m_x * xAxis.m_y - zAxis.m_y * xAxis.m_x);

	float (&raw)[16] = m_transform.m_rawData;
	raw[0] = m_scaleX * xAxis.m_x;
	raw[1] = m_scaleX * xAxis.m_y;
	raw[2] = m_scaleX * xAxis.m_z;
	raw[4] = m_scaleY * yAxis.m_x;
	raw[5] = m_scaleY * yAxis.m_y;
	raw[6] = m_scaleY * yAxis.m_z;
	raw[8] = m_scaleZ * zAxis.m_x;
	raw[9] = m_scaleZ * zAxis.m_y;
	raw[10] = m_scaleZ * zAxis.m_z;
	raw[12] = m_x;
	raw[13] = m_y;
	raw[14] = m_z;
	raw[3] = raw[7] = raw[11] = 0;
	raw[15] = 1;
	setTransform(m_transform);

	if (zAxis.m_z < 0)
	{
		setRotationY(180 - getRotationY());
		setRotationX(getRotationX() - 180);
		setRotationZ(getRotationZ() - 180);
	}
}

void Object3D::updateTransform()
{
	Vector3D elements[3];
	elements[0].setTo(m_x, m_y, m_z);
	elements[1].setTo(m_rotationX, m_rotationY, m_rotationZ);
	elements[2].setTo(m_scaleX, m_scaleY, m_scaleZ);
	m_transform.recompose(elements);

	m_transformDirty = false;
	m_positionDirty = false;
	m_rotationDirty = false;
	m_scaleDirty = false;
}

void Object3D::invalidatePosition()
{
	if (m_positionDirty)
		return;

	m_positionDirty = true;
	invalidateTransform();
}

void Object3D::invalidateRotation()
{
	if (m_rotationDirty)
		return;

	m_rotationDirty = true;
	invalidateTransform();
}

void Object3D::invalidateScale()
{
	if (m_scaleDirty)
		return;

	m_scaleDirty = true;
	invalidateTransform();
}