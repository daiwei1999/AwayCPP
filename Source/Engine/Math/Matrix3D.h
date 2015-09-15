#pragma once

#include "Common.h"
#include "Vector3D.h"

AWAY_NAMESPACE_BEGIN

enum class Orientation3D
{
	AXIS_ANGLE,
	EULER_ANGLES,
	QUATERNION
};

class Matrix3D
{
public:
	Matrix3D(float m00 = 1, float m10 = 0, float m20 = 0, float m30 = 0,
		     float m01 = 0, float m11 = 1, float m21 = 0, float m31 = 0,
		     float m02 = 0, float m12 = 0, float m22 = 1, float m32 = 0,
		     float m03 = 0, float m13 = 0, float m23 = 0, float m33 = 1);
	Matrix3D(const Matrix3D& matrix);

	void init(float m00, float m10, float m20, float m30,
		      float m01, float m11, float m21, float m31,
		      float m02, float m12, float m22, float m32,
		      float m03, float m13, float m23, float m33)
	{
		m_rawData[0] = m00; m_rawData[1] = m10; m_rawData[2] = m20; m_rawData[3] = m30;
		m_rawData[4] = m01; m_rawData[5] = m11; m_rawData[6] = m21; m_rawData[7] = m31;
		m_rawData[8] = m02; m_rawData[9] = m12; m_rawData[10] = m22; m_rawData[11] = m32;
		m_rawData[12] = m03; m_rawData[13] = m13; m_rawData[14] = m23; m_rawData[15] = m33;
	}

	void append(const Matrix3D& lhs, bool m33 = false);
	void appendRotation(float degrees, const Vector3D& axis, const Vector3D& pivotPoint = Vector3D::ZERO);
	void appendScale(float xScale, float yScale, float zScale);
	void appendTranslation(float x, float y, float z);

	void prepend(const Matrix3D& rhs);
	void prependRotation(float degrees, const Vector3D& axis);
	void prependScale(float xScale, float yScale, float zScale);
	void prependTranslation(float x, float y, float z);

	void copyColumnFrom(int column, const Vector3D& vector3D);
	void copyColumnTo(int column, Vector3D& vector3D) const;
	void copyFrom(const Matrix3D& sourceMatrix3D);
	void copyRawDataFrom(const float* data, int index = 0, bool transpose = false);
	void copyRawDataTo(float* data, int index = 0, bool transpose = false) const;
	void copyRowFrom(int row, const Vector3D& vector3D);
	void copyRowTo(int row, Vector3D& vector3D) const;
	void copyToMatrix3D(Matrix3D& dest) const;

	bool invert();
	void identity();
	void transpose();
	bool recompose(const Vector3D* components, Orientation3D orientationStyle = Orientation3D::EULER_ANGLES);
	void decompose(Vector3D* result, Orientation3D orientationStyle = Orientation3D::EULER_ANGLES) const;

	void getForward(Vector3D& result) const;
	void getUp(Vector3D& result) const;
	void getRight(Vector3D& result) const;

	void transformVectors(const float* vin, float* vout, int numPoints) const;

	static Vector3D* transformVector(const Matrix3D& matrix, const Vector3D* vin, Vector3D* vout);
	static Vector3D* deltaTransformVector(const Matrix3D& matrix, const Vector3D* vin, Vector3D* vout);

private:
	void getAxisRotation(float x, float y, float z, float degrees, Matrix3D& result) const;

public:
	float m_rawData[16];
};

AWAY_NAMESPACE_END