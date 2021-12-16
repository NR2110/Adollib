														#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace Physics_function {

		enum class Edgetype {
			EdgeConvex,	// 凸エッジ
			EdgeConcave,// 凹エッジ
			EdgeFlat,	// 平坦エッジ
		};

		//エッジ
		struct Edge {
			Edgetype type = Edgetype::EdgeConvex; // エッジの種類
			u_int vertexID[2] = { 0 }; // 端点の頂点インデックス vectorの再確保があるかもなのでIDで保存
			u_int facetID[2] = { 0 }; // 共有する面インデックス
		};

		//面
		struct Facet {
			u_int vertexID[3] = { 0 }; // 頂点インデックス
			u_int edgeID[3] = { 0 };   // エッジインデックス
			Vector3 normal;  // 面法線ベクトル
		};

		constexpr int involved_vertex_max = 8;
		struct Vertex_involvement {
			int edge_involvements[involved_vertex_max];
			int facet_involvements[involved_vertex_max];
		};

		struct Meshcollider_data{
			std::string FBX_pass;

			u_int index_num = 0;
			u_int vertex_num = 0; //頂点数
			u_int edge_num = 0; //エッジ数
			u_int facet_num = 0; //面数

			std::vector<int> indexes; //頂点情報
			std::vector<Vector3> vertices; //頂点情報
			std::vector<Vertex_involvement> vertex_involvements; //頂点の関わっているものへのindex
			std::vector<Edge> edges; //エッジ配列
			std::vector<Facet> facets; //面配列

			bool is_Convex = true; //convexなオブジェクトかどうか 一つでも凹なedgeがあればfalse
			DOP::DOP_14	dopbase; //初期k-dopのdata
			Vector3 base_pos[8];//初期k-dopの頂点データ
		};

		//struct Meshcoll_data {
		//	std::list<Meshcollider_data>::iterator collider_mesh;
		//};

	}
}