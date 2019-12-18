#include "Matrix.h"
#include <cassert>

namespace lag
{
	void crossAccum(const Vector &a, const Vector &b, Matrix &c)
	{
		assert(a.size() == Matrix::DIM);
		if(a.size() == 0) return ;
		assert(b.size() == Matrix::DIM);

		for(int i = 0 ; i < a.size(); i++)
		{
			for(int j = 0 ; j < b.size(); j++)
			{
				c[i][j] += a[i] * b[j];
			}
		}
	}

	Vector dot(const Vector &a, const Matrix &b)
	{
		assert(a.size() == Matrix::DIM);

		if(a.size() == 0 ) return Vector();

		Vector c(Matrix::DIM, 0);

		for(int i = 0 ; i < Matrix::DIM ; i++)
		{
			for(int j = 0 ; j < Matrix::DIM ; j++)
			{
				c[j] += a[i] * b[i][j]; 
			}
		}

		return c;
	}

	Matrix operator + (const Matrix &a, const Matrix&b)
	{
		assert(Matrix::DIM == Matrix::DIM);
		Matrix c(0.0);

		for(int i = 0 ; i < Matrix::DIM ;i++)
		{
			for(int j = 0 ; j < Matrix::DIM ; j++)
			{
				c[i][j] = a[i][j] + b[i][j];
			}
		}

		return c;
	}
}