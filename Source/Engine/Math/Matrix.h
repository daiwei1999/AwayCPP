#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Matrix
{
public:
	Matrix(float a = 1, float b = 0, float c = 0, float d = 1, float tx = 0, float ty = 0);

	void copyFrom(const Matrix& sourceMatrix);
	void identity();
	void invert();
	void rotate(float angle);
	void scale(float sx, float sy);
	void translate(float dx, float dy);

	static Point<float>* transformPoint(const Matrix& matrix, const Point<float>* pin, Point<float>* pout);
	
public:
	float m_a, m_b, m_c, m_d, m_tx, m_ty;
};

AWAY_NAMESPACE_END