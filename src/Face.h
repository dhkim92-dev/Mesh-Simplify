#ifndef _FACE_HEADER_
#define _FACE_HEADER_

#include "Edge.h"

namespace mesim 
{
	class Face{
		int v1,v2,v3;
		public :
		Face(int p, int q, int r) : v1(p), v2(q), v3(r) {}
		~Face(){}

		Edge against(int v) const;
		void reverse();
		void replace(int v, int v_new);
	};
};

#endif