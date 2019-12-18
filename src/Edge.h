#ifndef _EDGE_HEADER_
#define _EDGE_HEADER_

namespace mesim

{
	struct Edge{
		int v1,v2;

		Edge(int a = -1, int b = -1) : v1(a), v2(b) {}

		bool operator < (const Edge &e) const{
			if( v1 < e.v1) return true;
			else if(v1 == e.v1)
				return v2 < e.v2;
			else
				return false;
		}
	};
};

#endif

