
#include "meshcoll_resorce_manager.h"
#include <memory>
#include <wrl.h>
#include <map>
#include <string>
#include <vector>
#include <functional>


#pragma warning(push)
#pragma warning(disable:26812)
#pragma warning(disable:26495)
#include <fbxsdk.h>
#pragma warning(pop)

#include "../Math/crossing_func.h"

using namespace Adollib;
using namespace Physics_function;
using namespace Crossing_func;
using namespace std;

std::unordered_map <std::string, std::vector<Meshcollider_data>> Collider_ResourceManager::meshcoll_datas;

bool Collider_ResourceManager::CreateMCFromFBX(const char* fbxname, std::vector<Meshcollider_data>** ret_mesh, bool Right_triangle, bool is_permit_edge_have_many_facet) {

	//すでに同名ファイルをLoadしていれば
	if (meshcoll_datas.count((string)fbxname) == 1) {
		//for (auto& data : meshcoll_datas[fbxname]) {
		//	ret_mesh->collider_meshes.emplace_back(&data);
		//}
		*ret_mesh = &meshcoll_datas[fbxname];
		return true;
	}

	const char* fileName = fbxname;
	bool hr = true;

#pragma region Load FBX

	//FbxManagerとFbxSceneとFbxImporterオブジェクトを作成
	fbxsdk::FbxManager* manager = fbxsdk::FbxManager::Create();
	manager->SetIOSettings(fbxsdk::FbxIOSettings::Create(manager, IOSROOT));
	FbxImporter* importer = ::FbxImporter::Create(manager, "");
	bool import_status = false;
	import_status = importer->Initialize(fileName, -1, manager->GetIOSettings());
	//_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	FbxScene* scene = ::FbxScene::Create(manager, "");
	// データをインポート
	import_status = importer->Import(scene);
	//_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// 三角ポリゴン化
	FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);
	geometry_converter.RemoveBadPolygonsFromMeshes(scene);

	// ノード属性の取得
	vector<FbxNode*> fetched_meshes;
	function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute) {
				switch (fbx_node_attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());

	// メッシュデータの取得
	struct Subset {
		int index_start = 0;
		int index_count = 0;
		int vertex_start = 0;
		int vertex_count = 0;
	};
	std::vector<Subset> subsets;
	std::vector<Meshcollider_data>& _mesh = meshcoll_datas[fbxname];

	//TODO : FBX内のgloabal_transformを考慮していない

	_mesh.resize(fetched_meshes.size());
	subsets.resize(fetched_meshes.size());
	for (size_t mesh_num = 0; mesh_num < fetched_meshes.size(); mesh_num++)
	{
		FbxMesh* fbxMesh = fetched_meshes.at(mesh_num)->GetMesh();
		Subset& subset = subsets.at(mesh_num);
		Meshcollider_data& mesh = _mesh.at(mesh_num);
		mesh.FBX_pass = fbxname;
		//::

		Matrix44 mesh_globalTransform = matrix44_identity();
		// globalTransform
		FbxAMatrix global_transform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		for (int row = 0; row < 4; row++)
		{
			for (int column = 0; column < 4; column++)
			{
				mesh_globalTransform.m[row][column] = static_cast<float>(global_transform[row][column]);
			}
		}
		//::
		vector<int>& indices = mesh.indexes;
		vector<Vector3>& vertices = mesh.vertices;

		const FbxVector4* array_of_control_points = fbxMesh->GetControlPoints();
		const int number_of_polygons = fbxMesh->GetPolygonCount();

		indices.resize(number_of_polygons * 3);
		//vertices.resize(fbxMesh->GetControlPointsCount());

		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{
			// Where should I save the vertex attribute index, according to the material
			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {
				Vector3 vertex;
				// 頂点
				const int index_of_control_point = fbxMesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				//vertex = vector3_trans(vertex, mesh_globalTransform);

				// push_back
				int vertex_size = vertices.size();
				bool is_same_vertex = false;
				// 同じ頂点をmargeする場合
				for (int same_vertex_num = 0; same_vertex_num < vertex_size; ++same_vertex_num) {
					// 同じ頂点を探して
					if (vector3_distance(vertex, vertices[same_vertex_num]) < FLT_EPSILON) {
						indices.at(subset.index_start + index_of_vertex) = static_cast<u_int>(same_vertex_num); //indexに保存して

						// flagをtrueに
						is_same_vertex = true;
						break;
					}
				}
				// 同じ頂点が無ければ新たに保存
				if (is_same_vertex == false) {
					vertices.push_back(vertex);
					indices.at(subset.index_start + index_of_vertex) = static_cast<u_int>(vertex_size);
				}
			}
			subset.index_start += 3;
		}

#pragma endregion

#pragma region Set Meshcollider_mesh


		u_int& index_num = mesh.index_num;
		//u_int& vertex_num = mesh.vertex_num;

		std::vector<Vertex_involvement>& vertex_involvements = mesh.vertex_involvements;
		vector<Edge>& edges = mesh.edges;
		vector<Facet>& facets = mesh.facets;
		u_int& edge_num = mesh.edge_num;
		u_int& facet_num = mesh.facet_num;

		vertex_involvements.resize(vertices.size());
		bool& is_Convex = mesh.is_Convex;
		//::: vertexなどの情報からedge,facetの情報を計算 :::
		{
			index_num = indices.size();
			//面情報の保存
			{
				facet_num = index_num / 3;
				Physics_function::Facet F;


				for (u_int i = 0; i < facet_num; i++) {
					const int indexed[3] = {
						indices.at(i * 3 + 0),
						indices.at(i * 3 + 1),
						indices.at(i * 3 + 2)
					};

					const Vector3 normal = vector3_cross(vertices[indexed[1]] - vertices[indexed[0]], vertices[indexed[2]] - vertices[indexed[0]]);

					// 面の面積がとても小さければ面に追加しない
					if (normal.norm() < FLT_EPSILON) {
						continue;
					}

					if (Right_triangle) {
						F.vertexID[0] = indexed[0];
						F.vertexID[1] = indexed[1];
						F.vertexID[2] = indexed[2];
						F.normal = +normal;
						F.normal = F.normal.unit_vect();
					}
					else {
						F.vertexID[2] = indexed[0];
						F.vertexID[1] = indexed[1];
						F.vertexID[0] = indexed[2];
						F.normal = -normal;
						F.normal = F.normal.unit_vect();
					}

					// vertexからfacetへアクセスできるように保存
					vertex_involvements.at(indexed[0]).add_facet_involvment(facets.size());
					vertex_involvements.at(indexed[1]).add_facet_involvment(facets.size());
					vertex_involvements.at(indexed[2]).add_facet_involvment(facets.size());

					facets.emplace_back(F);
				}

				facet_num = facets.size();
			}

			//エッジ情報の保存
			{
				edge_num = 0;
				std::unordered_map<int, int> edgeID_Table;

				Physics_function::Edge E;
				for (u_int i = 0; i < facet_num; i++) {
					Physics_function::Facet& facet = facets[i];

					// 面の中で一番長いedgeを求める
					u_int longer_edge_id = 0;
					{
						float max_length = 0;
						for (int o = 0; o < 3; o++) {

							const u_int vertId0 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
							const u_int vertId1 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);

							if (max_length < vector3_distance(vertices[vertId0], vertices[vertId1])) {
								max_length = vector3_distance(vertices[vertId0], vertices[vertId1]);
								longer_edge_id = vertId0 + vertId1;
							};

						}
					}

					// edge情報を保存
					for (int o = 0; o < 3; o++) {
						const u_int vertId0 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						const u_int vertId1 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);

						const u_int b = vertId1 * vertId1 - vertId1;

						////int tableId = (vertId1 * vertId1 - vertId1) * 0.5f + vertId0;
						//float tableId_ = (intId1 * (intId1 - 1)) * 0.5f + intId0;

						// vertId0, vertId1から作られるユニークな数字
						int tableId = (int)(b + vertId0 * 2);

						if (edgeID_Table.count(tableId) == 0) {
							// 初回時は登録のみ
							E.facetID[0] = i;
							E.facetID[1] = i;
							E.vertexID[0] = vertId0;
							E.vertexID[1] = vertId1;
							E.type = Physics_function::Edgetype::EdgeConvex; // 凸エッジで初期化

							// vertexからedgeへアクセスできるように保存
							bool is_longer = false;
							if (longer_edge_id == vertId0 + vertId1)is_longer = true;
							vertex_involvements.at(vertId0).add_edge_involvment(edges.size(), is_longer);
							vertex_involvements.at(vertId1).add_edge_involvment(edges.size(), is_longer);

							edges.emplace_back(E);

							facet.edgeID[o] = edge_num;
							edgeID_Table[tableId] = edge_num;

							edge_num++;
						}
						else {
							// すでに登録されていた
							Physics_function::Edge& edge = edges[edgeID_Table[tableId]];
							Physics_function::Facet& facetB = facets[edge.facetID[0]];

							if (is_permit_edge_have_many_facet && edge.facetID[0] != edge.facetID[1]) {
								continue;
							}

							if (edge.facetID[0] != edge.facetID[1]) {
								int dafsgdhf = 0;

								auto& a = facets[edge.facetID[0]];
								auto& b = facets[edge.facetID[1]];
								int s = 0;
							}
							//assert(edge.facetID[0] == edge.facetID[1] && "Don't let the edges have 3～ facet.");


							// エッジに含まれないＡ面の頂点がB面の表か裏かで判断
							Vector3 s = vertices[facet.vertexID[(o + 2) % 3]];
							Vector3 q = vertices[facetB.vertexID[0]];
							float d = vector3_dot(s - q, facetB.normal);

							//エッジの種類を入力
							if (d < -FLT_EPSILON) {
								edge.type = Physics_function::Edgetype::EdgeConvex;
							}
							else if (d > FLT_EPSILON) {
								edge.type = Physics_function::Edgetype::EdgeConcave;
								is_Convex = false;
							}
							else {
								edge.type = Physics_function::Edgetype::EdgeFlat;
							}

							edge.facetID[1] = i;

							facet.edgeID[o] = edgeID_Table[tableId];
						}

					}


				}
			}


		}
#pragma endregion
#pragma region Set Dopbase
		//::
		DOP::DOP_14& dopbase = mesh.dopbase;

		for (int i = 0; i < DOP::DOP_size; i++) {
			dopbase.max[i] = -FLT_MAX;
			dopbase.min[i] = +FLT_MAX;
		}

		//初期状態のDOPの保存 loop中に計算するDOPはこのDOPを基準にする
		for (int i = 0; i < DOP::DOP_size; i++) {
			for (int& ind : indices) {

				float dis = vector3_dot(DOP::DOP_14_axis[i], vertices.at(ind));
				dopbase.max[i] = ALmax(dopbase.max[i], dis);
				dopbase.min[i] = ALmin(dopbase.min[i], dis);
			}
		}

		//{
		//	//角の4つのスラブを反転させる
		//	for (int axis_inv = 0; axis_inv < 2; ++axis_inv) {
		//		//角の4つのスラブ
		//		for (int axis = 0; axis < 4; ++axis) {
		//			int inv = axis_inv * -2 + 1;

		//			//角のスラブ
		//			const Vector3& plane_axis_A = DOP::DOP_14_axis[axis + 3];
		//			float plane_dis_A = (inv == 1 ? dopbase.max[axis + 3] : dopbase.min[axis + 3]);
		//			//plane_dis_A*= 0.5f;

		//			//x,y,zのスラブ
		//			float plane_dis_3[3]{
		//				{plane_axis_A.x * inv > 0 ? dopbase.max[0] : dopbase.min[0]},
		//				{plane_axis_A.y * inv > 0 ? dopbase.max[1] : dopbase.min[1]},
		//				{plane_axis_A.z * inv > 0 ? dopbase.max[2] : dopbase.min[2]}
		//			};

		//			//角のスラブとx,y,zのスラブの交点を求める
		//			for (int xyz = 0; xyz < 3; ++xyz) {
		//				getCrossingP_three_plane(
		//					DOP::DOP_14_axis[(0 + xyz) % 3], plane_dis_3[(0 + xyz) % 3],
		//					DOP::DOP_14_axis[(1 + xyz) % 3], plane_dis_3[(1 + xyz) % 3],
		//					plane_axis_A, plane_dis_A,
		//					mesh.base_pos[axis_inv * 12 + axis * 3 + xyz]
		//				);
		//				if (
		//					mesh.base_pos[axis_inv * 12 + axis * 3 + xyz].y > -200
		//					) {
		//					auto asd = DOP::DOP_14_axis[axis + 3].unit_vect();
		//					getCrossingP_three_plane(
		//						DOP::DOP_14_axis[(0 + xyz) % 3], plane_dis_3[(0 + xyz) % 3],
		//						DOP::DOP_14_axis[(1 + xyz) % 3], plane_dis_3[(1 + xyz) % 3],
		//						plane_axis_A, plane_dis_A,
		//						mesh.base_pos[axis_inv * 12 + axis * 3 + xyz]
		//					);
		//				}

		//			}



		//		}

		//	}



		//}

		{
			for (int x_ = 0; x_ < 2; x_++) {
				float x_dis = (x_ == 0 ? dopbase.max[0] : dopbase.min[0]);

				for (int y_ = 0; y_ < 2; y_++) {
					float y_dis = (y_ == 0 ? dopbase.max[1] : dopbase.min[1]);

					for (int z_ = 0; z_ < 2; z_++) {
						float z_dis = (z_ == 0 ? dopbase.max[2] : dopbase.min[2]);

						getCrossingP_three_plane(
							DOP::DOP_14_axis[0], x_dis,
							DOP::DOP_14_axis[1], y_dis,
							DOP::DOP_14_axis[2], z_dis,
							mesh.base_pos[z_ + y_ * 2 + x_ * 4]
						);



					}
				}
			}
		}



#pragma endregion

	}

	//meshcoll_datas[fbxname] = _mesh;


	//for (auto& data : meshcoll_datas[fbxname]) {
	//	ret_mesh->collider_meshes.emplace_back(&data);
	//}
	//*ret_mesh = &meshcoll_datas[fbxname];
	*ret_mesh = &meshcoll_datas[fbxname];

	manager->Destroy();

	return true;
}

void Collider_ResourceManager::destroy() {


}