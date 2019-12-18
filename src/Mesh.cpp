#include "Mesh.h"
#include "Matrix.h"
#include "Vector.h"

using namespace lag;

namespace mesim
{
	Mesh::Mesh(std::istream &in)
	{
		char c;
		int line = 0;
		int nr_face = 0;
		while(in.get(c))
		{	
			line++;
			if(c=='#'){
				char buf[1024];
				in.getline(buf, sizeof(buf));
				continue;
			}else if( c == '\n'){

			}else if( c == 'v'){
				// v , vn , vt 가 가능한데 일단 v 만 받아들이는 걸로 처리
				double x,y,z;
				in >> x >> y >> z;
				//std::cout <<"v : " << x << ", " << y << ", " << z << std::endl;
				verts.emplace_back(x,y,z); //오브젝트 구성 정점 정보 입력

			}else if( c == 'f'){
				int id[3];
				in >> id[0] >> id[1] >> id[2];
				//std::cout <<"f : " << id[0] << ", " << id[1] << ", " << id[2] << std::endl;
				if(vf_table.size() != verts.size())
				{
					faces.reserve(verts.size());
					vf_table.resize(verts.size());
					v_removed.resize(verts.size(), false);
					edges = new WaffleLink((int)verts.size());
				}

				id[0]--; id[1]--; id[2]--; // 페이스를 구성하는 정점인덱스 번호는 obj 파일 상에서 1부터 들어가므로 1을 빼준다.

				faces.emplace_back(id[0], id[1], id[2]); // 페이스 구성 버텍스 인덱스 입력
				//int sz_face = (int)faces.size();
	
				vf_table[id[0]].insert(nr_face); 
				vf_table[id[1]].insert(nr_face);
				vf_table[id[2]].insert(nr_face);
				nr_face++;

				//Garland & Heckbert 기반의 메쉬 간소화 알고리즘은 아래 조건을 만족해야한다.
				std::sort(id, id+3);
				assert( 0 <= id[0] && id[0] < id[1] && id[1] < id[2] && id[2] < verts.size());

				/*edges => WaffleLink*/

				edges->insert(id[0], id[1]);
				edges->insert(id[1], id[2]);
				edges->insert(id[0], id[2]);
			}else{
				std::cerr << "Not supported character -> " << c << " at line : " << line << std::endl;
				continue;
			}
		}

		nr_faces     = faces.size();
		nr_old_edges = edges->size();
		nr_old_faces = faces.size();
		nr_old_verts = verts.size();
	}

	Mesh::~Mesh()
	{
		verts.clear();
		v_removed.clear();
		faces.clear();
	}

	void Mesh::dumpFile(std::ostream &out) const 
	{
		size_t nr_v = verts.size();
		std::vector<int> vertex_id(nr_v, 0);
		int vtx_real = 0;

		for(int i = 0 ; i < nr_v ; i++){
			if(v_removed[i]) continue;
			vertex_id[i] = ++vtx_real;
			out << "v " << verts[i][0] << " " << verts[i][1] << " " << verts[i][2] << std::endl;
		}

		for(int i = 0 ; i < nr_v ; i++)
		{
			if(v_removed[i]) continue; // v_removed 가 true이면 제거된 버텍스이므로 출력하지 않는다.

			for(const auto &f_id : vf_table[i])
			{
				Edge edge = faces[f_id].against(i); 
				int v1 = edge.v1;
				int v2 = edge.v2;

				assert(!v_removed[v1] && !v_removed[v2]);
				assert(vertex_id[v1] && vertex_id[v2] && vertex_id[i]);

				if(i < v1 && i < v2)
					out << "f " << vertex_id[i] << " " << vertex_id[v1] << " " << vertex_id[v2] << std::endl;
			}
		}
	}

	// 엣지 e에 대해 split 할 새로운 버텍스 v_new 를 구함.
	std::pair<Vector, double> Mesh::getPosition(const Edge &e)
	{
		// 쿼드라틱 에러를 계산해야하므로 Q 매트릭스 생성
		Matrix Q(0.0);

		//아래는 Q 매트릭스 근사 함수입니다.
		auto calculate = [&](int v_id) -> void {
			for(const auto &f_id : vf_table[v_id]){
				Edge edge = faces[f_id].against(v_id);

				int v1 = edge.v1;
				int v2 = edge.v2;

				Vector normal_vec = cross(verts[v1] - verts[v_id], verts[v2] - verts[v_id]);
				normal_vec = normal_vec / norm(normal_vec);

				normal_vec.push_back(-dot(verts[v_id], normal_vec));
				crossAccum(normal_vec, normal_vec, Q);
			}
		};

		calculate(e.v1);
		calculate(e.v2);

		Vector v = (verts[e.v1] + verts[e.v2]) / 2;
		v.push_back(1); // 4차원으로 변환하여 Q 매트릭스와 dot product 하기 위하여 1을 추가

		double cost = dot( dot(v, Q), v); // 코스트 계산
		assert(cost > -EPS);

		v.pop_back(); // 3차원으로 다시 줄임.

		return std::make_pair(v, cost);
	}

	// Edge Collapse 대상이 될 엣지를 선택함.
	std::pair<Edge, Vector> Mesh::selectEdge(double threshold)
	{
		Edge idx(-1, -1);
		Vector pos;
		std::pair<double, Edge> tmp;


		// heap에서 코스트가 작은것을 먼저 가져옴.  
		// !!!!!!!!주의!!!!!!!!
		// 최소힙으로 사용하기위해 별도로 오퍼레이터 지정 안했음.
		// 코스트값은 -로 들어옴
		// heap -> p_q <pair < double, Edge>>
		while(!heap.empty())
		{
			tmp = heap.top();
			heap.pop();

			// 현재 가져온 엣지가 이미 삭제된 엣지라면 컨티뉴
			// tmp의 second에는 Edge의 양끝점 정보가 저장되어있음. Edge(int v1, int v2)
			if(!edges->find(tmp.second.v1, tmp.second.v2)) continue;
			// 버텍스가 제거된 엣지라면 컨티뉴
			if(v_removed[tmp.second.v1] || v_removed[tmp.second.v2]) continue;
			// 엣지 길이가 threshold보다 크면 컨티뉴
			if(edgeLen(tmp.second) > threshold) continue;

			std::pair<Vector, double> real = getPosition(tmp.second); // 합쳐질 정점을 구함.

			// 새로운 코스트와 기존의 코스트를 비교
			// act.second 는 아직 힙에 들어가지 않은 코스트이므로 양수이고, 
			// tmp.first 는 힙에서 가져온 코스트값이라 -가 곱해져 저장되어있다.
			// 따라서 그냥 더하는게 코스트의 차이.
			if(fabs(real.second + tmp.first) > EPS) continue;

			// 위 네개의 조건을 통과했다면 합침.

			idx = tmp.second; // 엣지(시작점 아이디, 끝점 아이디)
			pos = real.first; // 버텍스 좌표

			break;
		}
		// 제거될 엣지를 구하고 해당 엣지와 엣지 상의 새로운 버텍스 인덱스를 리턴

		return std::make_pair(idx, pos);
	}

	// 삼각형 방향 체크	 OpenGL, Vulkan 등에선 일반적으로 Front는 반시계방향

	bool Mesh::faceReverse(const Edge &e, const Vector& v1, const Vector& v2)
	{
		// Edge e -> 삭제될 엣지의 시작점을 포함하는 정점을 포함하는 삼각형에서 해당 정점과 마주보는 변
		// Vector& v1 삭제될 엣지 상의 시작점
		// Vector& v2 삭제될 엣지의 합쳐진 정점 좌표
		const Vector &x = verts[e.v1];
		const Vector &y = verts[e.v2];
		return dot(cross(x - v1, y - v1), cross(x-v2, y-v2)) < 0.0;
	}

	void Mesh::addToHeap(const Edge &e, double threshold)
	{
		if(edgeLen(e) > threshold) return; // 제거 쓰레시홀드보다 크면 힙에 추가 x

		std::pair<Vector,double> pos = getPosition(e); // 엣지 좌표 가져옴
		heap.emplace(-pos.second, e); // 최소힙으로 사용하기 위해 -값을 붙여 저장
	}

	void Mesh::updateEdge(int v, double threshold)
	{
		std::set<int> nearest;

		// 변수로 주어진 v를 포함하는 모든 페이스 아이디를 가져온다
		// 해당 페이스에서 정점 v를 마주보는 엣지를 가져온다.
		// 엣지의 양 끝점의 아이디를 nearest에 넣는다. // 이웃엣지
		for(const auto &f_id : vf_table[v])
		{
			Edge edge = faces[f_id].against(v); 
			nearest.insert(edge.v1);
			nearest.insert(edge.v2);
		}

		// 힙에 추가
		for(auto x : nearest)
		{
			addToHeap(Edge(min(x, v) , max(x, v)), threshold);
		}
	}

	void Mesh::removeEdge(const Edge &e, const Vector &v, double threshold)
	{
		// e -> 제거될 엣지
		// v -> 제거될 엣지의 양 끝점이 합쳐진 좌표.
		// v1을 포함하는 삼각형은 아래 반복문을 거치면서 삼각형 방향을 바꿈
		for(const auto &f_id : vf_table[e.v1]) // 삭제될 edge의 시작점을 포함하는 모든 삼각형 id에 대해 반복
		{
			Edge edge = faces[f_id].against(e.v1); // 삼각형 상의 e.v1과 마주보는 변을 가져옴

			if(edge.v1 == e.v2 || edge.v2 == e.v2) continue; // 엣지의 v1 == e.v2거나 edge.v2가 e.v2 라면 패스

			bool reverse = faceReverse(edge, verts[e.v1], v); // 삼각형의 방향이 바뀌었는지 확인
			if(!reverse) continue;

			assert(vf_table[edge.v2].find(f_id) != vf_table[edge.v2].end());
			assert(vf_table[edge.v1].find(f_id) != vf_table[edge.v1].end());

			faces[f_id].reverse(); // 삼각형 방향 뒤집음.
		}

		/*.    v2
			 /     \
            /       \
           /         \
          /           \
         /             \
		edge.v1 -- edge v2
		*/

		for(const auto &f_id : vf_table[e.v2])
		{
			Edge edge = faces[f_id].against(e.v2);

			auto reverse = faceReverse(edge, verts[e.v2], v); 
			assert( vf_table[edge.v2].find(f_id) != vf_table[edge.v2].end() );
			assert( vf_table[edge.v1].find(f_id) != vf_table[edge.v1].end() );

			if(edge.v1 != e.v1 && edge.v2 != e.v1)
			{
				faces[f_id].replace(e.v2, e.v1);
				vf_table[e.v1].insert(f_id);

				if(reverse){
					faces[f_id].reverse();
				}
			}else{
				vf_table[edge.v1].erase(f_id);
				vf_table[edge.v2].erase(f_id);
				nr_faces--;
			}

			if(edges->find(min(e.v2, edge.v1), max(e.v2, edge.v1)))
				edges->erase(min(e.v2, edge.v1), max(e.v2, edge.v1));
			if(edges->find(min(e.v2, edge.v2), max(e.v2, edge.v2)))
				edges->erase(min(e.v2, edge.v2), max(e.v2, edge.v2));
			if(edge.v1 != e.v1 && edge.v2 != e.v1)
			{
				edges->insert(min(e.v1, edge.v1), max(e.v1, edge.v1));
				edges->insert(min(e.v1, edge.v2), max(e.v1, edge.v2));
			}
		}

		// v1 - v2 를 v1으로 합침. v1에 저장하고 v2는 제거
		edges->erase(e.v1, e.v2);
		verts[e.v1] = v;
		verts[e.v2].clear();
		v_removed[e.v2] = true;
		vf_table[e.v2].clear();

		std::set<int> nearest; // 삼각형과 정점간의 연결 정보 업데이트

		for(const auto &f_id : vf_table[e.v1])
		{
			Edge edge = faces[f_id].against(e.v1);
			nearest.insert(edge.v1);
			nearest.insert(edge.v2);
		}

		for(auto nr : nearest)
		{
			updateEdge(nr, threshold);
		}
	}

	void Mesh::buildHeap(double threshold)
	{
		// 힙 초기화
		while(!heap.empty()) heap.pop();

		for(int i = 0 ; i < edges->size(); i++)
		{
			// WaffleLink::Node -> Node* l,r,u,d 그리고 x,y-> e.v1 e.v2
			WaffleLink::Node *node = edges->edgeX[i];
			while(node!=nullptr){
				addToHeap(Edge(node->x, node->y) , threshold);
				node = node -> dwn;
			}
		}
	}

	void Mesh::simplify(size_t target, double threshold)
	{
		buildHeap(threshold);

		while(nr_faces > target)
		{
			//selectEdge -> std::pair<Edge, Vector>
			// Edge class -> int v1,v2
			// e->pair (edge vector)
			auto e = selectEdge(threshold);
			if(e.first.v1 != -1) removeEdge(e.first, e.second, threshold);
			else
			{
				threshold *= 2.0;
				std::cout << "제거할 페이스 수가 부족하여 역치값을 두배 증가 시킵니다." << std::endl;
				buildHeap(threshold);
			}
		}
	}
};