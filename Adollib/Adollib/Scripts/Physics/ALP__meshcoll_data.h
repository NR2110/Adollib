#pragma once
#include "../Math/math.h"

namespace Adollib {
	namespace physics_function {

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

		struct Meshcoll_data {
			u_int vertex_num = 0; //頂点数
			u_int edge_num = 0; //エッジ数
			u_int facet_num = 0; //面数

			std::vector<Vector3>* vertices = nullptr; //頂点情報
			std::vector<Edge>* edges = nullptr; //エッジ配列
			std::vector<Facet>* facets = nullptr; //面配列

			bool is_Convex = true;
		};

	}
}