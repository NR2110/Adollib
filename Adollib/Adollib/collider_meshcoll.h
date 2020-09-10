#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"

namespace Adollib {

	enum class Edgetype {
		EdgeConvex,	// 凸エッジ
		EdgeConcave,// 凹エッジ
		EdgeFlat,	// 平坦エッジ
	};

	//エッジ
	struct Edge {
		Edgetype type; // エッジの種類
		u_int vertexID[2]; // 端点の頂点インデックス vectorの再確保があるかもなのでIDで保存
		u_int facetID[2]; // 共有する面インデックス 
	};

	//面
	struct Facet {
		u_int vertexID[3]; // 頂点インデックス
		u_int edgeID[3];   // エッジインデックス
		vector3 normal;  // 面法線ベクトル
	};


	//Mesh colliderクラス
	class Meshcoll : public Collider {
	public:
		vector3 half_size = vector3();
		vector3 offset = vector3();
		DOP::DOP_14 dopbase;

		u_int vertex_num; //頂点数
		u_int edge_num; //エッジ数
		u_int facet_num; //面数
		bool is_Convex = true;

		std::vector<vector3> vertices; //頂点情報
		std::vector<Edge> edges; //エッジ配列
		std::vector<Facet> facets; //面配列

		//不動オブジェクトとして生成
		Meshcoll(const char* filename, float density, vector3 pos = vector3(0, 0, 0)) {
			//shapeの設定
			shape = Collider_shape::shape_mesh;

			std::vector<int> inde; //頂点情報
			vertices.clear();
			edges.clear();
			facets.clear();

			Collider_ResourceManager::CreateMCFromFBX(filename, &inde, &vertices);

			//面情報の保存
			{
				facet_num = vertex_num / 3;
				Facet F;
				for (int i = 0; i < facet_num; i++) {
					F.vertexID[0] = inde[i * 3];
					F.vertexID[1] = inde[i * 3 + 1];
					F.vertexID[2] = inde[i * 3 + 2];

					F.normal = vector3_cross(vertices[F.vertexID[1]] - vertices[F.vertexID[0]], vertices[F.vertexID[2]] - vertices[F.vertexID[0]]);
					F.normal = F.normal.unit_vect();

					facets.emplace_back(F);
				}
			}

			//エッジ情報の保存
			{
				edge_num = 0;
				std::vector<u_int>edgeID_Table;
				edgeID_Table.resize(vertex_num * vertex_num / 2);
				for (u_int& u : edgeID_Table) {
					u = 0xffffff;
				}

				Edge E;
				for (int i = 0; i < facet_num; i++) {
					Facet& facet = facets[i];

					for (int o = 0; o < 3; o++) {
						u_int vertId0 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int vertId1 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int tableId = vertId1 * (vertId1 - 1) / 2 + vertId0;

						if (edgeID_Table[tableId] == 0xffffff) {
							// 初回時は登録のみ
							E.facetID[0] = i;
							E.facetID[1] = i;
							E.vertexID[0] = vertId0;
							E.vertexID[1] = vertId1;
							E.type = Edgetype::EdgeConvex; // 凸エッジで初期化
							edges.emplace_back(E);

							facet.edgeID[o] = edge_num;
							edgeID_Table[tableId] = edge_num;

							edge_num++;
						}
						else {
							// すでに登録されていた
							Edge& edge = edges[edgeID_Table[tableId]];
							Facet& facetB = facets[edge.facetID[0]];

							assert(edge.facetID[0] == edge.facetID[1] && "Don't let the edges have 3～ facet.");

							// エッジに含まれないＡ面の頂点がB面の表か裏かで判断
							vector3 s = vertices[facet.vertexID[(o + 2) % 3]];
							vector3 q = vertices[facetB.vertexID[0]];
							float d = vector3_dot(s - q, facetB.normal);

							//エッジの種類を入力
							if (d < -FLT_EPSILON) {
								edge.type = Edgetype::EdgeConvex;
							}
							else if (d > FLT_EPSILON) {
								edge.type = Edgetype::EdgeConcave;
								is_Convex = false;
							}
							else {
								edge.type = Edgetype::EdgeFlat;
							}

							edge.facetID[1] = i;
						}

					}


				}

			}

			memset(dopbase.max, -FLT_MAX, DOP::DOP_size);
			memset(dopbase.min,  FLT_MAX, DOP::DOP_size);

			//初期状態のDOPの保存 回転などを考慮したのDOPはこのDOPを基準にする
			for (int i = 0; i < DOP::DOP_size; i++) {
				for (vector3& v : vertices) {

					float dis = fabsf(vector3_dot(DOP::DOP_14_axis[i], v));
					if (dopbase.max[i] < dis) dop14.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
					if (dopbase.min[i] > dis) dop14.min[i] = +dis * 1.00000001f;
				}
			}

			half_size = vector3(dop14.max[0] - dop14.min[0], dop14.max[1] - dop14.min[1], dop14.max[2] - dop14.min[2]);

			//密度の保存
			this->density = density;

			//座標
			local_position = pos;

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//サイズの所得関数のオーバーライド
		quaternion get_dimension() const {
			return half_size;
		}
		//world変換関数のオーバーライド
		void update_world_trans();

		//sizeや密度が変更されると質量や完成モーメントの変更が必要になるからそのために用意
		void update_inertial(const vector3& half_size, float density = 1) {
			this->density = density;

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

		//dop6の更新
		void update_dop14();

	};
}