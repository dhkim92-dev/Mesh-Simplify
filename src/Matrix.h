#ifndef _MATRIX_HEADER_
#define _MATRIX_HEADER_

#include "Vector.h"

namespace lag
{
	class Matrix{
		private :
			double m[4][4];
		public :
		static const size_t DIM = 4;

		Matrix(double val)
		{
			for(int i = 0 ; i < DIM; i++)
			{
				for(int j = 0 ; j < DIM ; j++)
				{
					m[i][j] = val;
				}
			}
		}
		
		const double *operator [] (int row) const
		{
			return m[row];
		}


		double *operator [] (int row)
		{
			return m[row];
		}
	};

	void crossAccum(const Vector &a, const Vector &b, Matrix &c);
	Vector dot(const Vector &a, const Matrix &b);
	Matrix operator + (const Matrix &a, const Matrix&b);
}

#endif