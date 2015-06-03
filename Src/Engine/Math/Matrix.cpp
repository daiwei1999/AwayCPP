#include "Matrix.h"
#include "Point.h"

USING_AWAY_NAMESPACE

Matrix::Matrix(float a, float b, float c, float d, float tx, float ty)
{
	m_a = a;
	m_b = b;
	m_c = c;
	m_d = d;
	m_tx = tx;
	m_ty = ty;
}

void Matrix::copyFrom(const Matrix& sourceMatrix)
{
	m_a = sourceMatrix.m_a;
	m_b = sourceMatrix.m_b;
	m_c = sourceMatrix.m_c;
	m_d = sourceMatrix.m_d;
	m_tx = sourceMatrix.m_tx;
	m_ty = sourceMatrix.m_ty;
}

void Matrix::identity()
{
	m_a = m_d = 1;
	m_b = m_c = m_tx = m_ty = 0;
}

void Matrix::invert()
{
	float norm = m_a * m_d - m_b * m_c;
	if (norm == 0)
	{
		m_a = m_b = m_c = m_d = 0;
		m_tx = -m_tx;
		m_ty = -m_ty;
	}
	else
	{
		norm = 1 / norm;
		float a1 = m_d * norm;
		m_d = m_a * norm;
		m_a = a1;
		m_b *= -norm;
		m_c *= -norm;

		float tx1 = -m_a * m_tx - m_c * m_ty;
		m_ty = -m_b * m_tx - m_d * m_ty;
		m_tx = tx1;
	}
}

void Matrix::rotate(float angle)
{
	float cos = std::cos(angle);
	float sin = std::sin(angle);

	float a1 = m_a * cos - m_b * sin;
	m_b = m_a * sin + m_b * cos;
	m_a = a1;

	float c1 = m_c * cos - m_d * sin;
	m_d = m_c * sin + m_d * cos;
	m_c = c1;

	float tx1 = m_tx * cos - m_ty * sin;
	m_ty = m_tx * sin + m_ty * cos;
	m_tx = tx1;
}

void Matrix::scale(float sx, float sy)
{
	m_a *= sx;
	m_b *= sy;
	m_c *= sx;
	m_d *= sy;
	m_tx *= sx;
	m_ty *= sy;
}

void Matrix::translate(float dx, float dy)
{
	m_tx += dx;
	m_ty += dy;
}

Point* Matrix::transformPoint(const Matrix& matrix, const Point* pin, Point* pout)
{
	pout->m_x = pin->m_x * matrix.m_a + pin->m_y * matrix.m_c + matrix.m_tx;
	pout->m_y = pin->m_x * matrix.m_b + pin->m_y * matrix.m_d + matrix.m_ty;
	return pout;
}