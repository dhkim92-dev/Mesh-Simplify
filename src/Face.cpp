#include <cassert>
#include <algorithm>
#include "Face.h"

namespace mesim
{
	Edge Face::against(int v) const
	{
		assert(v==v1 || v==v2 || v==v3) ;

		if(v==v1)
			return Edge(v2, v3);
		else if(v==v2)
			return Edge(v3, v1);
		else
			return Edge(v1, v2);
	}

	void Face::reverse()
	{
		// Edge Collapse 방식으로 버텍스를 합치게되면 페이스를 구성하는 버텍스 순서가
		// 바뀌는 경우가 생기므로 그 때 사용하기 위한 함수
		std::swap(v1,v2); 
	}

	void Face::replace(int v, int v_new)
	{
		assert(v == v1 || v == v2 || v == v3);
		
		if(v==v1)
			v1 = v_new;
		else if(v==v2)
			v2 = v_new;
		else
			v3 = v_new;
	}
}