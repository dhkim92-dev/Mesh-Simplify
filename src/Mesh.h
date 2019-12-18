#ifndef _MESH_HEADER_
#define _MESH_HEADER_
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <queue>
#include <cassert>
#include <iostream>

#include "Vector.h"
#include "Edge.h"
#include "Face.h"
#include "WaffleLink.h"

#define BUFFER_SIZE 1024
#define INF 1e8
#define EPS 1e-8
//#define EPSLOOSE 0.1
//#define TOLERATE 2.0

using std::min;
using std::max;
using std::make_pair;

using namespace lag;

namespace mesim
{
	class Mesh
	{
		public :
			Mesh(std::istream &in);
			virtual ~Mesh();
			virtual void dumpFile(std::ostream &out) const; // object 파일 출력
			virtual void simplify(size_t remain_vert, double threshold);
			virtual inline size_t getFaces() const {return nr_faces;}
			virtual inline size_t getOldFaces() const {return nr_old_faces;}
			virtual inline size_t getOldEdges() const {return nr_old_edges;}
			virtual inline size_t getOldVertices() const {return nr_old_verts;}

		private :
			std::vector<Vector> verts;
			std::vector<bool> v_removed;
			std::vector<Face> faces;
			std::vector< std::set<int> > vf_table;

			WaffleLink *edges;

			std::priority_queue <std::pair<double, Edge> > heap;

			size_t nr_faces;
			size_t nr_old_faces;
			size_t nr_old_verts;
			size_t nr_old_edges;

			double edgeLen(const Edge &e)
			{
				return norm(verts[e.v1] - verts[e.v2]); // 엣지를 이루는 두 정점 사이의 거리 반환
			}

			// 엣지 컬랩스 결과로 얻어지는 새로운 위치를 얻어옴.
			std::pair<Vector, double> getPosition(const Edge &e);
			// 컬랩스 할 엣지를 선택
			std::pair<Edge, Vector> selectEdge(double threshold);

			/*
			엣지 컬랩스 결과로 페이스 법선 벡터의 방향이 변하는 경우가 있음.
			아래는 페이스의 방향을 체크해두는 함수.   v1
			v1 -> 업데이트 이전 버텍스          /   \
			v2 -> 새로운 버텍스.             /     \
			e -> v1의 반대편에 있는 엣지.     /       \
			                            x--------y
			*/
			bool faceReverse(const Edge &e, const Vector &v1, const Vector &v2);

			// 엣지를 힙에 추가.
			void addToHeap(const Edge &e, double threshold);
			// 이웃 엣지들을 업데이트하고 힙에 추가
			void updateEdge(int v, double threshold);

			/*
			아래는 엣지를 메쉬에서 제거하는 함수
			e => 제거될 엣지
			v => 엣지 컬랩스로 생성된 v1 위치를 대체할 버텍스
			threshold : 제거될 엣지의 최대 길이
			*/

			void removeEdge(const Edge& e, const Vector &v, double threshold);
			void buildHeap(double threshold);

	};
};
#endif