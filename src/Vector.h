#ifndef _VECTOR_HEADER_
#define _VECTOR_HEADER_

#include <algorithm>
#include <cmath>

namespace lag
{
	class Vector
	{
		public :
		double p[4]; // x,y,z,w 좌표
		int s; // 몇개의 좌표를 사용하는지 체크, 예) x,y,z -> s 3 , x,y,z,w -> 4


		/* 생성자*/
		public :
		Vector()
		{
			s=0;
		} 
		
		Vector(int val)
		{
			s = val;
		}
		
		Vector(double x, double y, double z) 
		{
			p[0] = x; 
			p[1] = y;
			p[2] = z;
			s=3;
		}

		Vector(int n , double x)
		{
			s = n;
			for(int i = 0 ; i < n ; i++)
				p[i] = x;
		}

		int size() const {return s;}
		void clear(){s =0;}
		
		void push_back(double x)
		{
			if(s<=4)
				p[s++] = x;
		}

		void pop_back()
		{
			if(s>=0)
				s--;
		}

		void swap(Vector &a)
		{
			for(int i = 0 ; i < s ; i++)
			{
				std::swap(p[i], a.p[i]);
			}
		}
		double& operator [] (int x) {return p[x];}
		const double& operator[] (int x) const {return p[x];}
	};


		/* 연산자 정의 */
	Vector operator + (const Vector &a, const Vector &b);
	Vector operator - (const Vector &a, const Vector &b);
	Vector operator * (const double &a, const Vector &b);
	Vector operator / (const Vector &a, const double &b);

	/*벡터 연산 정의*/
	double norm(const Vector &a); // l2 distance
	Vector cross(const Vector &a, const Vector &b); // 외적
	double dot(const Vector &a, const Vector &b); // 내적	
}

#endif