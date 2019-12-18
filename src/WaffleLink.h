#ifndef CROSS_LINK_HEADER
#define CROSS_LINK_HEADER

#include <cassert>

namespace mesim
{
	class WaffleLink
	{
		public :
		struct Node {
	        Node *lhs, *rhs, *ups, *dwn;
	        int x; // 엣지의 첫번째 정점
	        int y; // 엣지의 두번째 정점
	        
	        Node(int x, int y) : x(x), y(y) {
	            lhs = rhs = ups = dwn = nullptr;
	        }
	    };

	    int s;
	    Node **edgeX;
	    Node **edgeY;
		WaffleLink(int size) : s(size) 
		{
			edgeX = new Node*[size];
			edgeY = new Node*[size];

			for(int i = 0 ; i < size ; i++)
			{
				edgeX[i] = edgeY[i] = nullptr;
			}
		}

		~WaffleLink()
		{
			for(int i = 0 ; i < s ; ++i)
			{
				Node *node = edgeX[i];
				while(node != nullptr)
				{
					Node *next = node -> dwn;
					delete node;
					node = next;
				}
			}
			delete[] edgeX;
			delete[] edgeY;
		}

		inline int size() const {return s;}

		bool insert(int x, int y)
		{
			Node *p = edgeX[x];
			Node *node = new Node(x,y);

			if(p == nullptr) {
				edgeX[x] = node; // 빈경우 헤드노드 설정
			}else{
				while( p != nullptr ){
					if(p->y > y){
						// 현재 생성된 엣지의 시작점이 x이다.
						// 엣지 x에 연결된 엣지들의 연결리스트 edgeX[x]
						// 새로운 노드의 y좌표가 p->y 보다 작다면
						// node 가 edgeX[x]가 가리키는 노드의 위쪽에 삽입된다.
						node->dwn = edgeX[x];
						edgeX[x]->ups = node;
						edgeX[x] = node;
						break;
					}else if (p->y == y){
						// 동일한 엣지가 이미 있으므로 삽입을 하지 않고
						// 생성한 노드의 메모리를 회수
						delete node;
						return true;
					}else{
						// y의 값이 더 크다면
						if(p->dwn == nullptr){
							p->dwn = node; // 헤드에서 밑이 없다면 바로 추가
							node->ups = p;
							break;
						}

						else if(p->dwn->y > y){
							node->dwn = p->dwn; // 더 큰 값을 갖는 다음 노드의 뒤쪽에다 붙임.
							node->ups = p;
							p->dwn->ups = node;
							p->dwn = node;
							break;
						}else{
							p = p->dwn;
						}
					}
				}
			}

			p = edgeY[y];

			if(p==nullptr){ // 빈 경우
				edgeY[y] = node; // 끝점 기준 헤드로 설정
			}else{
				while(p!=nullptr){
					if(p->x > x){ // 현재 포인터가 가리키는 x좌표가 더 큰 경우
						node->rhs = edgeY[y]; //  이하 헤드의 왼쪽에 붙임
						edgeY[y]->lhs = node;
						edgeY[y] = node;
						break;
					}else if(p->x == x){
						delete node; // x와 마찬가지
						return true; 
					}else{ // 새로운 노드의 시작 엣지 아이디가 더 큰 경우
						if(p->rhs == nullptr){
							p->rhs = node; // 우측이 빈 경우 바로 삽입
							node->lhs = p; // 새로운 노드의 좌측 노드는 p가 가리키는 노드
							break;
						}else if(p->rhs->x > x){
							node -> rhs = p->rhs; // 현재 탐색 노드의 우측에 위치한 노드의 엣지 시작점이 더 큰 경우
							node->lhs = p; // 새 노드의 좌측에 현재 탐색 노드를 연결
							p->rhs->lhs = node; // 탐색 노드 다음 노드의 좌측에 현재 노드를 연결
							p->rhs = node; // 탐색 노드의 우측 노드를 새 노드에 연결
							break;
						}else{
							p = p->rhs; // 다음 노드로 탐색 
						}
					}
				}
			}
			return false;
		}

		bool find(int x, int y)
		{
			Node *p = edgeX[x];

			if( p == nullptr ){
				return false;
			}else{
				while(p != nullptr){
					if(p->y > y){
						return false;
					}else if(p->y == y){
						return true;
					}else{
						if(p->dwn == nullptr){
							return false;
						}else if(p->dwn->y > y){
							return false;
						}else{
							p = p->dwn;
						}
					}
				}
			}

			return false;
		}

		//아래는 그냥 삭제 링크드 리스트랑 같다.

		void erase(int x, int y)
		{
			Node *p = edgeX[x];

			while( p!= nullptr){
				if(p->y > y){
					break;
				}else if(p-> y == y){
					if(p->lhs != nullptr){
						p->lhs->rhs = p->rhs;
					}else{
						assert(p == edgeY[y]);
						edgeY[y] = p->rhs;
					}

					if(p->rhs != nullptr){
						p->rhs->lhs = p->lhs;
					}

					if(p->ups != nullptr){
						p->ups->dwn = p->dwn;
					}else{
						assert(p==edgeX[x]);
						edgeX[x] = p->dwn;
					}

					if(p->dwn != nullptr){
						p->dwn->ups = p->ups;
					}

					delete p;
					break;
				}

				else{
					if(p->dwn == nullptr){
						break;
					}else if(p->dwn->y > y){
						break;
					}else{
						p = p->dwn;
					}
				}
			}
		}
	};
};
#endif