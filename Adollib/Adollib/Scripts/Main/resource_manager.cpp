

#include <memory>
#include <wrl.h>
#include <map>
#include <string>
#include <fbxsdk.h>
#include <vector>
#include <functional>
#include "../../DirectXTK-master/Inc/WICTextureLoader.h"
#include "misc.h"
#include "systems.h"
#include "resource_manager.h"
using namespace std;
using namespace DOP;

namespace Adollib
{
	std::unordered_map<std::string, std::vector<Mesh::mesh>> ResourceManager::meshes;
	std::unordered_map<std::wstring, Texture> ResourceManager::texturs;
	std::unordered_map<std::string, ResourceManager::VS_resorce>   ResourceManager::VSshaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>     ResourceManager::PSshaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>>  ResourceManager::GSshaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11HullShader>>      ResourceManager::HSshaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11DomainShader>>    ResourceManager::DSshaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>>   ResourceManager::CSshaders;

#pragma region Shader load

	HRESULT ResourceManager::CreateVsFromCso(
		const char* csoName,	           // 頂点シェーダーファイル名
		ID3D11VertexShader** vertexShader, // 頂点シェーダーオブジェクトのポインタの格納先
		ID3D11InputLayout** inputLayout,   // 入力レイアウトオブジェクトのポインタの格納先
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, // 頂点データ
		UINT numElements)                  // 頂点データの要素数
	{
		VS_resorce VS;
		if (VSshaders.count((string)csoName) == 1) {
			VS = VSshaders[(string)csoName];
		}
		else {
			HRESULT hr;

			// 頂点シェーダーファイルを開く
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// 頂点シェーダーファイルのサイズを求める
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// メモリ上にシェーダーデータを読み込む
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // 用意した領域にデータを読み込む
			fclose(fp);

			// 頂点シェーダーオブジェクトを生成する
			hr = Systems::Device->CreateVertexShader(
				cso_data.get(),  // 頂点シェーダーデータのポインタ
				cso_sz,		     // 頂点シェーダーデータのサイズ
				nullptr,
				&VS.VSshader
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// 入力レイアウトオブジェクトの生成
			hr = Systems::Device->CreateInputLayout(
				inputElementDesc, // 頂点データの内容
				numElements,	  // 頂点データの要素数
				cso_data.get(),	  // 頂点シェーダーデータ（input_element_descの内容と sprite_vs.hlslの内容に不一致がないかチェックするため）
				cso_sz,			  // 頂点シェーダーデータサイズ
				&VS.layout		  // 入力レイアウトオブジェクトのポインタの格納先。
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			VSshaders[(string)csoName] = VS;
		}
		*vertexShader = VS.VSshader.Get();
		*inputLayout  = VS.layout.Get();

		return S_OK;
	}


	HRESULT ResourceManager::CreatePsFromCso(
		const char* csoName, // ピクセルシェーダーファイル名
		ID3D11PixelShader** pixel_shader // 格納するポインタ
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*pixel_shader = PSshaders[(string)csoName].Get();
		}
		else {
			// ピクセルシェーダーファイルを開く
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// ピクセルシェーダーファイルのサイズを求める
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// メモリ上にピクセルシェーダーデータを格納する領域を用意する
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // 用意した領域にデータを読み込む
			fclose(fp);

			// 生成
			HRESULT hr = Systems::Device->CreatePixelShader(
				cso_data.get(), // ピクセルシェーダーデータのポインタ
				cso_sz,			// ピクセルシェーダーデータサイズ
				nullptr,
				pixel_shader    // ピクセルシェーダーオブジェクトのポインタの格納先
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			PSshaders[(string)csoName] = *pixel_shader;
		}

		return S_OK;
	}


	HRESULT ResourceManager::CreateGsFromCso(
		const char* csoName, // ピクセルシェーダーファイル名
		ID3D11GeometryShader** geometry_shader // 格納するポインタ
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*geometry_shader = GSshaders[(string)csoName].Get();
		}
		else {
			// シェーダーファイルを開く
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// ピクセルシェーダーファイルのサイズを求める
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// メモリ上にシェーダーデータを格納する領域を用意する
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // 用意した領域にデータを読み込む
			fclose(fp);

			// 生成
			HRESULT hr = Systems::Device->CreateGeometryShader(
				cso_data.get(), // シェーダーデータのポインタ
				cso_sz,			// シェーダーデータサイズ
				nullptr,
				geometry_shader    // シェーダーオブジェクトのポインタの格納先
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			GSshaders[(string)csoName] = *geometry_shader;
		}

		return S_OK;
	}

	HRESULT ResourceManager::CreateHsFromCso(
		const char* csoName, // ピクセルシェーダーファイル名
		ID3D11HullShader** hull_shader // 格納するポインタ
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*hull_shader = HSshaders[(string)csoName].Get();
		}
		else {
			// シェーダーファイルを開く
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// ピクセルシェーダーファイルのサイズを求める
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// メモリ上にシェーダーデータを格納する領域を用意する
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // 用意した領域にデータを読み込む
			fclose(fp);

			// 生成
			HRESULT hr = Systems::Device->CreateHullShader(
				cso_data.get(), // シェーダーデータのポインタ
				cso_sz,			// シェーダーデータサイズ
				nullptr,
				hull_shader    // シェーダーオブジェクトのポインタの格納先
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			HSshaders[(string)csoName] = *hull_shader;
		}

		return S_OK;
	}

	HRESULT ResourceManager::CreateDsFromCso(
		const char* csoName, // ピクセルシェーダーファイル名
		ID3D11DomainShader** domain_shader // 格納するポインタ
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*domain_shader = DSshaders[(string)csoName].Get();
		}
		else {
			// シェーダーファイルを開く
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// ピクセルシェーダーファイルのサイズを求める
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// メモリ上にシェーダーデータを格納する領域を用意する
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // 用意した領域にデータを読み込む
			fclose(fp);

			// 生成
			HRESULT hr = Systems::Device->CreateDomainShader(
				cso_data.get(), // シェーダーデータのポインタ
				cso_sz,			// シェーダーデータサイズ
				nullptr,
				domain_shader    // シェーダーオブジェクトのポインタの格納先
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			DSshaders[(string)csoName] = *domain_shader;
		}

		return S_OK;
	}

	HRESULT ResourceManager::CreateCsFromCso(
		const char* csoName, // ピクセルシェーダーファイル名
		ID3D11ComputeShader** compute_shader // 格納するポインタ
	)
	{
		if (CSshaders.count((string)csoName) == 1) {

			*compute_shader = CSshaders[(string)csoName].Get();

		}
		else {
			//コンピュートシェーダーファイルを開く
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// ピクセルシェーダーファイルのサイズを求める
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// メモリ上にピクセルシェーダーデータを格納する領域を用意する
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // 用意した領域にデータを読み込む
			fclose(fp);

			// 生成
			HRESULT hr = Systems::Device->CreateComputeShader(
				cso_data.get(), // ピクセルシェーダーデータのポインタ
				cso_sz,			// ピクセルシェーダーデータサイズ
				nullptr,
				compute_shader    // ピクセルシェーダーオブジェクトのポインタの格納先
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			CSshaders[(string)csoName] = *compute_shader;
		}

		return S_OK;
	}

#pragma endregion

	// テクスチャの読み込み
#pragma region Load Texture

	HRESULT ResourceManager::LoadTextureFromFile(
		const wchar_t* fileName,
		ID3D11ShaderResourceView** pSRV, 
		D3D11_TEXTURE2D_DESC* pTex2dDesc
	)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;

		if(texturs.count((std::wstring)fileName) == 1)
		{
			pSRV = texturs[(std::wstring)fileName].ShaderResourceView.GetAddressOf();
		}
		else
		{
			Texture& tex = texturs[(std::wstring)fileName];

			// 見つからなかった場合は生成する
			hr = DirectX::CreateWICTextureFromFile(Systems::Device.Get(), fileName, resource.GetAddressOf(), &tex.ShaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			pSRV = &tex.ShaderResourceView;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// テクスチャリースのプロパティを取得する
			texture2d->GetDesc(&tex.texture2d_desc);
			pTex2dDesc = &tex.texture2d_desc;
		}

		return hr;
	}
#pragma endregion
	
	// モデルの読み込み
	HRESULT ResourceManager::CreateModelFromFBX(vector<Mesh::mesh>** ret_mesh, const char* fileName, const char* filePath)
	{
		//すでにロード済みであればそのアドレスを返す
		if (meshes.count((string)filePath + (string)fileName) == 1) {
			*ret_mesh = &meshes[(string)filePath + (string)fileName];
			return S_OK;
		}

		HRESULT hr = S_OK;
		vector<Mesh::mesh> skinMeshes;

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

		// ノード属性の取得　(現在はメッシュのみ)
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
		vector<Mesh::VertexFormat> vertices;
		vector<u_int> indices;
		u_int vertex_count = 0;

		skinMeshes.resize(fetched_meshes.size());
		for (size_t mesh_num = 0; mesh_num < fetched_meshes.size(); mesh_num++)
		{
			FbxMesh* fbxMesh = fetched_meshes.at(mesh_num)->GetMesh();
			Mesh::mesh& mesh = skinMeshes.at(mesh_num);

			// globalTransform
			FbxAMatrix global_transform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
			for (int row = 0; row < 4; row++)
			{
				for (int column = 0; column < 4; column++)
				{
					mesh.globalTransform.m[row][column] = static_cast<float>(global_transform[row][column]);
				}
			}

			FbxStringList uv_names;
			fbxMesh->GetUVSetNames(uv_names);

			// マテリアルの取得
			const int number_of_materials = fbxMesh->GetNode()->GetMaterialCount();
			mesh.subsets.resize(number_of_materials);
			if (number_of_materials > 0)
			{
				// Count the faces of each material
				const int number_of_polygons = fbxMesh->GetPolygonCount();
				for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
				{
					const u_int material_index = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
					mesh.subsets.at(material_index).indexCount += 3;
				}
				// Record the offset (how many vertex)
				int offset = 0;
				for (Mesh::subset& sb : mesh.subsets)
				{
					sb.indexStart = offset;
					offset += sb.indexCount;
					// This will be used as counter in the following procedures, reset to zero
					sb.indexCount = 0;
				}
			}
			for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
			{
				Mesh::subset& subset = mesh.subsets.at(index_of_material);
				const FbxSurfaceMaterial* surface_material = fbxMesh->GetNode()->GetMaterial(index_of_material);
				const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
				const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
				if (property.IsValid() && factor.IsValid())
				{
					FbxDouble3 color = property.Get<FbxDouble3>();
					double f = factor.Get<FbxDouble>();
					mesh.subsets.at(index_of_material).diffuse.color.x = static_cast<float>(color[0] * f);
					mesh.subsets.at(index_of_material).diffuse.color.y = static_cast<float>(color[1] * f);
					mesh.subsets.at(index_of_material).diffuse.color.z = static_cast<float>(color[2] * f);
					mesh.subsets.at(index_of_material).diffuse.color.w = 1.0f;
				}
				if (property.IsValid())
				{
					const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
					if (number_of_textures)
					{
						const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
						if (file_texture)
						{
							// texname
							const char* filename = file_texture->GetRelativeFileName();

							string name = filename;

							bool isFind = FALSE;
							while (!isFind)
							{
								// 無視する
								if (name.find("png") != string::npos ||
									name.find("jpg") != string::npos)
								{
									int n = name.rfind("\\");
									if (n != -1)
										name = name.substr(n + 1);
									isFind = TRUE;
								}
							}

							string texPath = (string)filePath + (string)name;
							setlocale(LC_ALL, "japanese");
							wchar_t texName[MAX_PATH] = { 0 };
							size_t ret = 0;
							mbstowcs_s(&ret, texName, MAX_PATH, texPath.c_str(), _TRUNCATE);

							D3D11_TEXTURE2D_DESC texture2D_Desc = {};
							hr = ResourceManager::LoadTextureFromFile(texName, mesh.subsets.at(index_of_material).diffuse.shaderResourceVirw.GetAddressOf(), &texture2D_Desc);
							_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

							// ピクセルシェーダーオブジェクトの生成 
						//	ResourceManager::CreatePsFromCso("./DefaultShader/default_ps.cso", mesh.subsets.at(index_of_material).pixelShader.GetAddressOf());
						}
					}
					else
					{
						// ピクセルシェーダーオブジェクトの生成 
					//	ResourceManager::CreatePsFromCso("./DefaultShader/default_ps.cso", mesh.subsets.at(index_of_material).pixelShader.GetAddressOf());
					}
				}
			}

			// アニメーションの取得
			fetch_animations(fbxMesh, mesh.skeletalAnimation);

			// ボーンのウエイトの取得
			vector<Mesh::bone_influences_per_control_point> bone_influences;
			fetch_bone_influences(fbxMesh, bone_influences);

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
				Mesh::subset& subset = mesh.subsets.at(index_of_material);
				const int index_offset = subset.indexStart + subset.indexCount;
				for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++) {
					Mesh::VertexFormat vertex;
					// 頂点
					const int index_of_control_point = fbxMesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
					vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
					vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
					vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);


					// 法線
					if (fbxMesh->GetElementNormalCount())
					{
						FbxVector4 normal;
						fbxMesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
						vertex.normal.x = static_cast<float>(normal[0]);
						vertex.normal.y = static_cast<float>(normal[1]);
						vertex.normal.z = static_cast<float>(normal[2]);
					}
					// UV
					if (fbxMesh->GetElementUVCount())
					{
						FbxVector2 uv;
						bool unmapped_uv;
						fbxMesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
						vertex.texcoord.x = static_cast<float>(uv[0]);
						vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
					}
					// Weight
					if (fbxMesh->GetDeformerCount(FbxDeformer::eSkin) && bone_influences.size() > index_of_control_point)
					{
						for (int bone_influence_count = 0; bone_influence_count < bone_influences[index_of_control_point].size(); bone_influence_count++)
						{
							if (bone_influence_count < MAX_BONE_INFLUENCES)
							{
								vertex.bone_weights[bone_influence_count] = bone_influences[index_of_control_point][bone_influence_count].weight;
								vertex.bone_indices[bone_influence_count] = bone_influences[index_of_control_point][bone_influence_count].index;
							}
						}
					}
					// push_back
					vertices.push_back(vertex);
					indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
					vertex_count++;
				}
				subset.indexCount += 3;
			}

			//DOP_14の登録
			{

				for (int i = 0; i < DOP_size; i++) {
					mesh.dop7.max[i] = -FLT_MAX;
					mesh.dop7.min[i] = +FLT_MAX;
				}
				for (int ver = 0; ver < vertices.size(); ver++) {
					for (int axi = 0; axi < DOP_size; axi++) {
						float V = vector3_dot(DOP_14_axis[axi].unit_vect(), vertices[ver].position);

						if (mesh.dop7.max[axi] < V)mesh.dop7.max[axi] = V;
						if (mesh.dop7.min[axi] > V)mesh.dop7.min[axi] = V;
					}
				}
			}


#pragma endregion

			// 頂点情報・インデックス情報のセット
#pragma region Set VertexData and IndexData

		// 頂点バッファの設定
			D3D11_BUFFER_DESC vertexDesc = {};
			vertexDesc.ByteWidth = sizeof(Mesh::VertexFormat) * vertices.size();   // バッファのサイズ	
			vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;	        // バッファの読み書き法
			vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // パイプラインにどうバインドするか指定
			vertexDesc.CPUAccessFlags = 0;                      // CPUのアクセスフラグ　0でアクセスしない
			vertexDesc.MiscFlags = 0;                           // その他のフラグ
			vertexDesc.StructureByteStride = 0;                 // バッファ構造体の場合の要素数

			// 頂点バッファに頂点データを入れるための設定
			D3D11_SUBRESOURCE_DATA vertexSubResource = {};
			vertexSubResource.pSysMem = &vertices[0];   // 初期化データのポインタ
			vertexSubResource.SysMemPitch = 0;          // 頂点バッファでは使わない
			vertexSubResource.SysMemSlicePitch = 0;     // 頂点バッファでは使わない

			// 頂点バッファの生成
			hr = Systems::Device->CreateBuffer(&vertexDesc, &vertexSubResource, mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#pragma endregion

			// インデックスバッファオブジェクトの生成
#pragma region Create IndexBuffer

			// index
			//CreateIndices(fbxMesh, &indices);

			D3D11_BUFFER_DESC indexDesc = {};
			indexDesc.ByteWidth = indices.size() * sizeof(u_int);          // バッファのサイズ	
			indexDesc.Usage = D3D11_USAGE_IMMUTABLE;	          // バッファの読み書き法
			indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;    // パイプラインにどうバインドするか指定
			indexDesc.CPUAccessFlags = 0;                    // CPUのアクセスフラグ　0でアクセスしない
			indexDesc.MiscFlags = 0;                           // その他のフラグ
			indexDesc.StructureByteStride = 0;                 // バッファ構造体の場合の要素数

			D3D11_SUBRESOURCE_DATA indexSubResource = {};
			indexSubResource.pSysMem = &indices[0];   // 初期化データのポインタ
			indexSubResource.SysMemPitch = 0;        // 頂点バッファでは使わない
			indexSubResource.SysMemSlicePitch = 0;   // 頂点バッファでは使わない

			hr = Systems::Device->CreateBuffer(&indexDesc, &indexSubResource, mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

#pragma endregion

		 //FBXロード完了
		meshes[(string)filePath + (string)fileName] = skinMeshes;
		*ret_mesh = &meshes[(string)filePath + (string)fileName];

		//scene->Destroy();
		//importer->Destroy();
		manager->Destroy();

		return hr;
	}


	// アニメーションを取得する
	void ResourceManager::fetch_animations(FbxMesh* fbx_mesh, vector<Mesh::skeletal_animation>& skeletal_animation,
		u_int sampling_rate)
	{
		// Get the list of all the animation stack.
		FbxArray<FbxString*> array_of_animation_stack_names;
		fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);
		// Get the number of animations.
		int number_of_animations = array_of_animation_stack_names.Size();
		if (number_of_animations > 0)
		{
			skeletal_animation.resize(number_of_animations);
			for (int i = 0; i < number_of_animations; i++)
			{
				// Get the FbxTime per animation's frame.
				FbxTime::EMode time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
				FbxTime frame_time;
				frame_time.SetTime(0, 0, 0, 1, 0, time_mode);
				sampling_rate = sampling_rate > 0 ? sampling_rate : frame_time.GetFrameRate(time_mode);
				float sampling_time = 1.0f / sampling_rate;
				skeletal_animation.at(i).sampling_time = sampling_time;
				skeletal_animation.at(i).animation_tick = 0.0f;
				FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(i);
				FbxAnimStack* current_animation_stack
					= fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
				fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);
				FbxTakeInfo* take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
				FbxTime start_time = take_info->mLocalTimeSpan.GetStart();
				FbxTime end_time = take_info->mLocalTimeSpan.GetStop();
				FbxTime sampling_step;
				sampling_step.SetTime(0, 0, 1, 0, 0, time_mode);
				sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_time);
				for (FbxTime current_time = start_time; current_time < end_time; current_time += sampling_step)
				{
					Mesh::skeletal skeletal;
					fetch_bone_matrices(fbx_mesh, skeletal, current_time);
					skeletal_animation.at(i).push_back(skeletal);
				}
			}
		}
		for (int i = 0; i < number_of_animations; i++)
		{
			delete array_of_animation_stack_names[i];
		}
	}


	// ボーン行列を取得する
	void ResourceManager::fetch_bone_matrices(FbxMesh* fbx_mesh, vector<Mesh::bone>& skeletal, FbxTime time)
	{
		const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
		{
			FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
			const int number_of_clusters = skin->GetClusterCount();
			skeletal.resize(number_of_clusters);
			for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
			{
				Mesh::bone& bone = skeletal.at(index_of_cluster);
				FbxCluster* cluster = skin->GetCluster(index_of_cluster);
				// this matrix trnasforms coordinates of the initial pose from mesh space to global space
				FbxAMatrix reference_global_init_position;
				cluster->GetTransformMatrix(reference_global_init_position);
				// this matrix trnasforms coordinates of the initial pose from bone space to global space
				FbxAMatrix cluster_global_init_position;
				cluster->GetTransformLinkMatrix(cluster_global_init_position);
				// this matrix trnasforms coordinates of the current pose from bone space to global space
				FbxAMatrix cluster_global_current_position;
				cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(time);
				// this matrix trnasforms coordinates of the current pose from mesh space to global space
				FbxAMatrix reference_global_current_position;
				reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(time);
				// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
				// (translation, rotation and scale), the last row of the matrix represents the translation part of the
				// transformation.
				FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position
					* cluster_global_init_position.Inverse() * reference_global_init_position;
				// convert FbxAMatrix(transform) to XMDLOAT4X4(bone.transform)
				for (int row = 0; row < 4; row++)
				{
					for (int column = 0; column < 4; column++)
					{
						bone.transform.m[row][column] = static_cast<float>(transform[row][column]);
					}
				}
			}
		}
	}

	// ボーンのウエイトを取得する
	// FBXMesh*                                    読み込むFBXMesh
	// vector<bone_influences_per_control_point>&  取得したウエイトの保存先
	void ResourceManager::fetch_bone_influences(const FbxMesh* fbx_mesh, vector<Mesh::bone_influences_per_control_point>& influences)
	{
		const int number_of_control_points = fbx_mesh->GetControlPointsCount();
		influences.resize(number_of_control_points);
		const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer) {
			FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
			const int number_of_clusters = skin->GetClusterCount();
			for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster) {
				FbxCluster* cluster = skin->GetCluster(index_of_cluster);
				const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();
				const int* array_of_control_point_indices = cluster->GetControlPointIndices();
				const double* array_of_control_point_weights = cluster->GetControlPointWeights();
				for (int i = 0; i < number_of_control_point_indices; ++i) {
					Mesh::bone_influences_per_control_point& influences_per_control_point
						= influences.at(array_of_control_point_indices[i]);
					Mesh::bone_influence influence;
					influence.index = index_of_cluster;
					influence.weight = static_cast<float>(array_of_control_point_weights[i]);
					influences_per_control_point.push_back(influence);
				}
			}
		}
	}


	void ResourceManager::destroy() {


	}
}