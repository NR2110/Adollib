
#include "meshcoll_resorce_manager.h"
#include <memory>
#include <wrl.h>
#include <map>
#include <string>
#include <fbxsdk.h>
#include <vector>
#include <functional>

#include "../../DirectXTK-master/Inc/WICTextureLoader.h"
#include "../Math/crossing_func.h"
#include "../Main/misc.h"
#include "../Main/systems.h"

using namespace Adollib;
using namespace physics_function;
using namespace Crossing_func;
using namespace std;

std::unordered_map <std::string, std::vector<Meshcollider_mesh>> Collider_ResourceManager::meshes;

bool Collider_ResourceManager::CreateMCFromFBX(const char* fbxname, std::vector<Meshcollider_mesh>** ret_mesh) {

	if (meshes.count((string)fbxname) == 1) {
		*ret_mesh = &meshes[fbxname];
		return true;
	}
	const char* fileName = fbxname;
	bool hr = true;

#pragma region Load FBX

	//FbxManager��FbxScene��FbxImporter�I�u�W�F�N�g���쐬
	FbxManager* manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
	FbxImporter* importer = FbxImporter::Create(manager, "");
	bool import_status = false;
	import_status = importer->Initialize(fileName, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	FbxScene* scene = FbxScene::Create(manager, "");
	// �f�[�^���C���|�[�g
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// �O�p�|���S����
	FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);

	// �m�[�h�����̎擾
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

	// ���b�V���f�[�^�̎擾
	u_int vertex_count = 0;
	struct Subset {
		int start;
		int count;
	};
	std::vector<Subset> subsets;
	std::vector<Meshcollider_mesh> _mesh;
	vector<Vector3> l_vertices;

	//TODO : FBX����gloabal_transform���l�����Ă��Ȃ� 

	_mesh.resize(fetched_meshes.size());
	subsets.resize(fetched_meshes.size());
	for (size_t mesh_num = 0; mesh_num < fetched_meshes.size(); mesh_num++)
	{
		FbxMesh* fbxMesh = fetched_meshes.at(mesh_num)->GetMesh();
		Subset& subset = subsets.at(mesh_num);
		Meshcollider_mesh& mesh = _mesh.at(mesh_num);
		//::
		vector<int>& indices = mesh.indexes;

		// �}�e���A���̎擾
		const int number_of_materials = fbxMesh->GetNode()->GetMaterialCount();

		const FbxVector4* array_of_control_points = fbxMesh->GetControlPoints();
		const int number_of_polygons = fbxMesh->GetPolygonCount();
		indices.resize(number_of_polygons * 3);
		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{
			// Where should I save the vertex attribute index, according to the material
			const int index_offset = subset.start + subset.count;
			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {
				Vector3 vertex;
				// ���_
				const int index_of_control_point = fbxMesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				// push_back
				l_vertices.push_back(vertex);
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count++;
			}
			subset.count += 3;
		}

#pragma endregion
#pragma region Set Meshcollider_mesh
		vector<Vector3>& vertices = mesh.vertices;

		u_int& index_num = mesh.index_num;
		u_int& vertex_num = mesh.vertex_num;

		vector<Edge>& edges = mesh.edges;
		vector<Facet>& facets = mesh.facets;
		u_int& edge_num = mesh.edge_num;
		u_int& facet_num = mesh.facet_num;

		bool& is_Convex = mesh.is_Convex;
		//::: vertex�Ȃǂ̏�񂩂�edge,facet�̏����v�Z :::
		{
			index_num = indices.size();
			vertices.resize(index_num);
			//���_����ۑ�
			{
				for (u_int i = 0; i < index_num; i++) {
					vertices[i] = l_vertices[indices[i]];
				}
			}

			vertex_num = vertices.size();
			//�ʏ��̕ۑ�
			{
				facet_num = vertex_num / 3;
				physics_function::Facet F;
				for (u_int i = 0; i < facet_num; i++) {
					//F.vertexID[0] = indices[i * 3];
					//F.vertexID[1] = indices[i * 3 + 1];
					//F.vertexID[2] = indices[i * 3 + 2];
					F.vertexID[0] = i * 3;
					F.vertexID[1] = i * 3 + 1;
					F.vertexID[2] = i * 3 + 2;

					F.normal = vector3_cross(vertices[F.vertexID[1]] - vertices[F.vertexID[0]], vertices[F.vertexID[2]] - vertices[F.vertexID[0]]);
					F.normal = F.normal.unit_vect();

					facets.emplace_back(F);
				}
			}

			//�G�b�W���̕ۑ�
			{
				edge_num = 0;
				std::vector<int>edgeID_Table;
				edgeID_Table.resize(vertex_num * vertex_num / 2);
				for (int& u : edgeID_Table) {
					u = 0xffffff;
				}

				physics_function::Edge E;
				for (u_int i = 0; i < facet_num; i++) {
					physics_function::Facet& facet = facets[i];

					for (int o = 0; o < 3; o++) {
						u_int vertId0 = ALmin(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						u_int vertId1 = ALmax(facet.vertexID[o % 3], facet.vertexID[(o + 1) % 3]);
						int tableId = (int)((int)vertId1 * ((int)vertId1 - 1) / (float)2 + (int)vertId0);

						if (edgeID_Table[tableId] == 0xffffff) {
							// ���񎞂͓o�^�̂�
							E.facetID[0] = i;
							E.facetID[1] = i;
							E.vertexID[0] = vertId0;
							E.vertexID[1] = vertId1;
							E.type = physics_function::Edgetype::EdgeConvex; // �ʃG�b�W�ŏ�����
							edges.emplace_back(E);

							facet.edgeID[o] = edge_num;
							edgeID_Table[tableId] = edge_num;

							edge_num++;
						}
						else {
							// ���łɓo�^����Ă���
							physics_function::Edge& edge = edges[edgeID_Table[tableId]];
							physics_function::Facet& facetB = facets[edge.facetID[0]];

							assert(edge.facetID[0] == edge.facetID[1] && "Don't let the edges have 3�` facet.");

							// �G�b�W�Ɋ܂܂�Ȃ��`�ʂ̒��_��B�ʂ̕\�������Ŕ��f
							Vector3 s = vertices[facet.vertexID[(o + 2) % 3]];
							Vector3 q = vertices[facetB.vertexID[0]];
							float d = vector3_dot(s - q, facetB.normal);

							//�G�b�W�̎�ނ����
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
		}
#pragma endregion
#pragma region Set Dopbase
		//::
		DOP::DOP_14& dopbase = mesh.dopbase;

		for (int i = 0; i < DOP::DOP_size; i++) {
			dopbase.max[i] = -FLT_MAX;
			dopbase.min[i] = +FLT_MAX;
		}

		//������Ԃ�DOP�̕ۑ� loop���Ɍv�Z����DOP�͂���DOP����ɂ���
		for (int i = 0; i < DOP::DOP_size; i++) {
			for (Vector3& v : vertices) {

				float dis = vector3_dot(DOP::DOP_14_axis[i], v);
				if (dopbase.max[i] < dis) dopbase.max[i] = +dis * 1.00000001f;//�m���ɂ��邽�߂�����Ƒ傫�߂ɂƂ�
				if (dopbase.min[i] > dis) dopbase.min[i] = +dis * 1.00000001f;
			}
		}

		{
			//�p��4�̃X���u�𔽓]������
			for (int axis_inv = 0; axis_inv < 2; ++axis_inv) {
				//�p��4�̃X���u
				for (int axis = 0; axis < 4; ++axis) {
					int inv = axis_inv * -2 + 1;

					//�p�̃X���u
					const Vector3& plane_axis_A = DOP::DOP_14_axis[axis + 3];
					float plane_dis_A = (inv == 1 ? dopbase.max[axis + 3] : dopbase.min[axis + 3]);

					//x,y,z�̃X���u
					float plane_dis_3[3]{
						{plane_axis_A.x * inv > 0 ? dopbase.max[0] : dopbase.min[0]},
						{plane_axis_A.y * inv > 0 ? dopbase.max[1] : dopbase.min[1]},
						{plane_axis_A.z * inv > 0 ? dopbase.max[2] : dopbase.min[2]}
					};

					//�p�̃X���u��x,y,z�̃X���u�̌�_�����߂�
					for (int xyz = 0; xyz < 3; ++xyz) {
						getCrossingP_three_plane(
							plane_axis_A, plane_dis_A,
							DOP::DOP_14_axis[(0 + xyz) % 3], plane_dis_3[(0 + xyz) % 3],
							DOP::DOP_14_axis[(1 + xyz) % 3], plane_dis_3[(1 + xyz) % 3],
							mesh.base_pos[axis_inv * 12 + axis * 3 + xyz]
						);

					}



				}

			}
		}



#pragma endregion
		//::
		Vector3& halfsize = mesh.half_size;
		halfsize = Vector3(dopbase.max[0] - dopbase.min[0], dopbase.max[1] - dopbase.min[1], dopbase.max[2] - dopbase.min[2]) / 2.0f;
	}

	meshes[fbxname] = _mesh;
	*ret_mesh = &meshes[fbxname];


	return true;
}

void Collider_ResourceManager::destroy() {


}