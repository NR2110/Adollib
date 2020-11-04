#pragma once
#include "collider__base.h"
#include "meshcoll_resorce_manager.h"
#include "ALP__meshcoll_data.h"

namespace Adollib {

	//Mesh colliderクラス
	class Meshcoll : public Collider {
	public:
		Vector3 center;
		Vector3 rotate;
		Vector3 size;

	private:
		u_int vertex_num; //頂点数
		u_int edge_num; //エッジ数
		u_int facet_num; //面数
		bool is_Convex = true; //convexなオブジェクトかどうか 一つでも凹なedgeがあればfalse

		std::vector<Vector3>* vertices; //頂点情報
		std::vector<physics_function::Edge> edges; //エッジ配列
		std::vector<physics_function::Facet> facets; //面配列

	public:
		//不動オブジェクトとして生成
		Meshcoll() :center(Vector3(0)), rotate(Vector3(0)), size(0) {}

		void load_mesh(const char* filename) {
			////shapeの設定
			//shape = Collider_shape::shape_mesh;

			vertices = nullptr;
			edges.clear();
			facets.clear();

			std::vector<int>* inde; //頂点情報

			Collider_ResourceManager::CreateMCFromFBX(filename, &inde, &vertices);

			std::vector<Vector3>& vert = (*vertices);

			vertex_num = vert.size();

			//面情報の保存
			{
				facet_num = vertex_num / 3;
				physics_function::Facet F;
				for (int i = 0; i < facet_num; i++) {
					F.vertexID[0] = (*inde)[i * 3];
					F.vertexID[1] = (*inde)[i * 3 + 1];
					F.vertexID[2] = (*inde)[i * 3 + 2];

					F.normal = vector3_cross(vert[F.vertexID[1]] - vert[F.vertexID[0]], vert[F.vertexID[2]] - vert[F.vertexID[0]]);
					F.normal = F.normal.unit_vect();

					facets.emplace_back(F);
				}
			}

			//エッジ情報の保存
			{
				edge_num = 0;
				std::vector<int>edgeID_Table;
				edgeID_Table.resize(vertex_num * vertex_num / 2);
				for (int& u : edgeID_Table) {
					u = 0xffffff;
				}

				physics_function::Edge E;
				for (int i = 0; i < facet_num; i++) {
					physics_function::Facet& facet = facets[i];

					for (int o = 0; o < 3; o++) {
						u_int vertId0 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int vertId1 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						int tableId = (int)((int)vertId1 * ((int)vertId1 - 1) / (float)2 + (int)vertId0);

						if (edgeID_Table[tableId] == 0xffffff) {
							// 初回時は登録のみ
							E.facetID[0] = i;
							E.facetID[1] = i;
							E.vertexID[0] = vertId0;
							E.vertexID[1] = vertId1;
							E.type = physics_function::Edgetype::EdgeConvex; // 凸エッジで初期化
							edges.emplace_back(E);

							facet.edgeID[o] = edge_num;
							edgeID_Table[tableId] = edge_num;

							edge_num++;
						}
						else {
							// すでに登録されていた
							physics_function::Edge& edge = edges[edgeID_Table[tableId]];
							physics_function::Facet& facetB = facets[edge.facetID[0]];

							assert(edge.facetID[0] == edge.facetID[1] && "Don't let the edges have 3～ facet.");

							// エッジに含まれないＡ面の頂点がB面の表か裏かで判断
							Vector3 s = vert[facet.vertexID[(o + 2) % 3]];
							Vector3 q = vert[facetB.vertexID[0]];
							float d = vector3_dot(s - q, facetB.normal);

							//エッジの種類を入力
							if (d < -FLT_EPSILON) {
								edge.type = physics_function::Edgetype::EdgeConvex;
							}
							else if (d > FLT_EPSILON) {
								edge.type = physics_function::Edgetype::EdgeConcave;
								is_Convex = false;
							}
							else {
								edge.type = physics_function::Edgetype::EdgeFlat;
							}

							edge.facetID[1] = i;
						}

					}


				}

			}

			for (int i = 0; i < DOP::DOP_size; i++) {
				dopbase.max[i] = -FLT_MAX;
				dopbase.min[i] = +FLT_MAX;
			}

			//初期状態のDOPの保存 回転などを考慮したのDOPはこのDOPを基準にする
			for (int i = 0; i < DOP::DOP_size; i++) {
				for (Vector3& v : vert) {

					float dis = vector3_dot(DOP::DOP_14_axis[i], v);
					if (dopbase.max[i] < dis) dopbase.max[i] = +dis * 1.00000001f;//確実にするためちょっと大きめにとる
					if (dopbase.min[i] > dis) dopbase.min[i] = +dis * 1.00000001f;
				}
			}

			half_size = Vector3(dopbase.max[0] - dopbase.min[0], dopbase.max[1] - dopbase.min[1], dopbase.max[2] - dopbase.min[2]) / 2.0f;

			//質量の計算
			inertial_mass = (half_size.x * half_size.y * half_size.z) * 8.0f * density;

			//慣性モーメントの計算
			inertial_tensor = matrix_identity();
			inertial_tensor._11 = 0.3333333f * inertial_mass * ((half_size.y * half_size.y) + (half_size.z * half_size.z));
			inertial_tensor._22 = 0.3333333f * inertial_mass * ((half_size.z * half_size.z) + (half_size.x * half_size.x));
			inertial_tensor._33 = 0.3333333f * inertial_mass * ((half_size.x * half_size.x) + (half_size.y * half_size.y));
		}

	};
}