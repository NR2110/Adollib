
#include "meshcoll_resorce_manager.h"
#include <memory>
#include <wrl.h>
#include <map>
#include <string>
#include <fbxsdk.h>
#include <vector>
#include <functional>
#include "DirectXTK-master/Inc/WICTextureLoader.h"
#include "misc.h"

#include "systems.h"

using namespace Adollib;
using namespace std;
std::unordered_map <std::string, std::vector<int>>	Collider_ResourceManager::indexes;
std::unordered_map <std::string, std::vector<vector3>>Collider_ResourceManager::vertexes;

HRESULT Collider_ResourceManager::CreateMCFromFBX(const char* fbxname, std::vector<int>** out_indexes, std::vector<vector3>** out_vertexes) {

	if (indexes.count((string)fbxname) == 1) {
		*out_indexes = &indexes[(string)fbxname];
		*out_vertexes = &vertexes[(string)fbxname];
		return S_OK;
	}
	const char* fileName = fbxname;
	HRESULT hr = S_OK;

#pragma region Load FBX

	//FbxManagerとFbxSceneとFbxImporterオブジェクトを作成
	FbxManager* manager = FbxManager::Create();
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
	FbxImporter* importer = FbxImporter::Create(manager, "");
	bool import_status = false;
	import_status = importer->Initialize(fileName, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	FbxScene* scene = FbxScene::Create(manager, "");
	// データをインポート
	import_status = importer->Import(scene);
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	// 三角ポリゴン化
	FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*replace*/true);

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
	vector<vector3> vertices;
	vector<int> indices;
	u_int vertex_count = 0;
	struct Subset {
		int start;
		int count;
	};
	std::vector<std::vector<Subset>> subsets;

	subsets.resize(fetched_meshes.size());
	for (size_t mesh_num = 0; mesh_num < fetched_meshes.size(); mesh_num++)
	{
		FbxMesh* fbxMesh = fetched_meshes.at(mesh_num)->GetMesh();
		std::vector<Subset>& subset = subsets.at(mesh_num);

		// マテリアルの取得
		const int number_of_materials = fbxMesh->GetNode()->GetMaterialCount();

		subset.resize(number_of_materials);
		const FbxVector4* array_of_control_points = fbxMesh->GetControlPoints();
		const int number_of_polygons = fbxMesh->GetPolygonCount();
		indices.resize(number_of_polygons * 3);
		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{

			// The material for current face.
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}
			// Where should I save the vertex attribute index, according to the material
			const int index_offset = subset[index_of_material].start + subset[index_of_material].count;
			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {
				vector3 vertex;
				// 頂点
				const int index_of_control_point = fbxMesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				// push_back
				vertices.push_back(vertex);
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count++;
			}
			subset[index_of_material].count += 3;
		}

		vertexes[(string)fbxname] = vertices;
		indexes[(string)fbxname] = indices;

		*out_indexes = &indexes[(string)fbxname];
		*out_vertexes = &vertexes[(string)fbxname];


#pragma endregion
	}
}

void Collider_ResourceManager::destroy() {


}