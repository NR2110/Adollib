

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
#include "resource_manager.h"
using namespace std;

namespace Adollib
{
	std::map<std::string, std::vector<Mesh::mesh>> ResourceManager::meshes;
	std::map<std::wstring, Texture> ResourceManager::texturs;
	std::map<std::string, ResourceManager::VS_resorce>   ResourceManager::VSshaders;
	std::map<std::string, ID3D11PixelShader*>    ResourceManager::PSshaders;
	std::map<std::string, ID3D11GeometryShader*> ResourceManager::GSshaders;
	std::map<std::string, ID3D11HullShader*>     ResourceManager::HSshaders;
	std::map<std::string, ID3D11DomainShader*>   ResourceManager::DSshaders;

#pragma region Shader load

	HRESULT ResourceManager::CreateVsFromCso(
		const char* csoName,	           // ���_�V�F�[�_�[�t�@�C����
		ID3D11VertexShader** vertexShader, // ���_�V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
		ID3D11InputLayout** inputLayout,   // ���̓��C�A�E�g�I�u�W�F�N�g�̃|�C���^�̊i�[��
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc, // ���_�f�[�^
		UINT numElements)                  // ���_�f�[�^�̗v�f��
	{
		VS_resorce VS;
		if (VSshaders.count((string)csoName) == 1) {
			VS = VSshaders[(string)csoName];
		}
		else {
			HRESULT hr;

			// ���_�V�F�[�_�[�t�@�C�����J��
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// ���_�V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// ��������ɃV�F�[�_�[�f�[�^��ǂݍ���
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // �p�ӂ����̈�Ƀf�[�^��ǂݍ���
			fclose(fp);

			// ���_�V�F�[�_�[�I�u�W�F�N�g�𐶐�����
			hr = Systems::Device->CreateVertexShader(
				cso_data.get(),  // ���_�V�F�[�_�[�f�[�^�̃|�C���^
				cso_sz,		     // ���_�V�F�[�_�[�f�[�^�̃T�C�Y
				nullptr,
				&VS.VSshader
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// ���̓��C�A�E�g�I�u�W�F�N�g�̐���
			hr = Systems::Device->CreateInputLayout(
				inputElementDesc, // ���_�f�[�^�̓��e
				numElements,	  // ���_�f�[�^�̗v�f��
				cso_data.get(),	  // ���_�V�F�[�_�[�f�[�^�iinput_element_desc�̓��e�� sprite_vs.hlsl�̓��e�ɕs��v���Ȃ����`�F�b�N���邽�߁j
				cso_sz,			  // ���_�V�F�[�_�[�f�[�^�T�C�Y
				&VS.layout		  // ���̓��C�A�E�g�I�u�W�F�N�g�̃|�C���^�̊i�[��B
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			VSshaders[(string)csoName] = VS;
		}
		*vertexShader = VS.VSshader;
		*inputLayout  = VS.layout;

		return S_OK;
	}


	HRESULT ResourceManager::CreatePsFromCso(
		const char* csoName, // �s�N�Z���V�F�[�_�[�t�@�C����
		ID3D11PixelShader** pixel_shader // �i�[����|�C���^
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*pixel_shader = PSshaders[(string)csoName];
		}
		else {
			// �s�N�Z���V�F�[�_�[�t�@�C�����J��
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// �s�N�Z���V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // �p�ӂ����̈�Ƀf�[�^��ǂݍ���
			fclose(fp);

			// ����
			HRESULT hr = Systems::Device->CreatePixelShader(
				cso_data.get(), // �s�N�Z���V�F�[�_�[�f�[�^�̃|�C���^
				cso_sz,			// �s�N�Z���V�F�[�_�[�f�[�^�T�C�Y
				nullptr,
				pixel_shader    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			PSshaders[(string)csoName] = *pixel_shader;
		}

		return S_OK;
	}


	HRESULT ResourceManager::CreateGsFromCso(
		const char* csoName, // �s�N�Z���V�F�[�_�[�t�@�C����
		ID3D11GeometryShader** geometry_shader // �i�[����|�C���^
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*geometry_shader = GSshaders[(string)csoName];
		}
		else {
			// �s�N�Z���V�F�[�_�[�t�@�C�����J��
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// �s�N�Z���V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // �p�ӂ����̈�Ƀf�[�^��ǂݍ���
			fclose(fp);

			// ����
			HRESULT hr = Systems::Device->CreateGeometryShader(
				cso_data.get(), // �s�N�Z���V�F�[�_�[�f�[�^�̃|�C���^
				cso_sz,			// �s�N�Z���V�F�[�_�[�f�[�^�T�C�Y
				nullptr,
				geometry_shader    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			GSshaders[(string)csoName] = *geometry_shader;
		}

		return S_OK;
	}

	HRESULT ResourceManager::CreateHsFromCso(
		const char* csoName, // �s�N�Z���V�F�[�_�[�t�@�C����
		ID3D11HullShader** hull_shader // �i�[����|�C���^
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*hull_shader = HSshaders[(string)csoName];
		}
		else {
			// �s�N�Z���V�F�[�_�[�t�@�C�����J��
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// �s�N�Z���V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // �p�ӂ����̈�Ƀf�[�^��ǂݍ���
			fclose(fp);

			// ����
			HRESULT hr = Systems::Device->CreateHullShader(
				cso_data.get(), // �s�N�Z���V�F�[�_�[�f�[�^�̃|�C���^
				cso_sz,			// �s�N�Z���V�F�[�_�[�f�[�^�T�C�Y
				nullptr,
				hull_shader    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			HSshaders[(string)csoName] = *hull_shader;
		}

		return S_OK;
	}

	HRESULT ResourceManager::CreateDsFromCso(
		const char* csoName, // �s�N�Z���V�F�[�_�[�t�@�C����
		ID3D11DomainShader** domain_shader // �i�[����|�C���^
	)
	{
		if (PSshaders.count((string)csoName) == 1) {
			*domain_shader = DSshaders[(string)csoName];
		}
		else {
			// �s�N�Z���V�F�[�_�[�t�@�C�����J��
			FILE* fp = nullptr;
			fopen_s(&fp, csoName, "rb");
			_ASSERT_EXPR_A(fp, "CSO File not found");

			// �s�N�Z���V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
			fseek(fp, 0, SEEK_END);
			long cso_sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// ��������Ƀs�N�Z���V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
			unique_ptr<unsigned char[]> cso_data = make_unique<unsigned char[]>(cso_sz);
			fread(cso_data.get(), cso_sz, 1, fp); // �p�ӂ����̈�Ƀf�[�^��ǂݍ���
			fclose(fp);

			// ����
			HRESULT hr = Systems::Device->CreateDomainShader(
				cso_data.get(), // �s�N�Z���V�F�[�_�[�f�[�^�̃|�C���^
				cso_sz,			// �s�N�Z���V�F�[�_�[�f�[�^�T�C�Y
				nullptr,
				domain_shader    // �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̃|�C���^�̊i�[��
			);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			DSshaders[(string)csoName] = *domain_shader;
		}

		return S_OK;
	}
#pragma endregion

	// �e�N�X�`���̓ǂݍ���
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

			// ������Ȃ������ꍇ�͐�������
			hr = DirectX::CreateWICTextureFromFile(Systems::Device.Get(), fileName, resource.GetAddressOf(), &tex.ShaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			pSRV = &tex.ShaderResourceView;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
			hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			// �e�N�X�`�����[�X�̃v���p�e�B���擾����
			texture2d->GetDesc(&tex.texture2d_desc);
			pTex2dDesc = &tex.texture2d_desc;
		}

		return hr;
	}
#pragma endregion
	
	// ���f���̓ǂݍ���
	HRESULT ResourceManager::CreateModelFromFBX(vector<Mesh::mesh>** ret_mesh, const char* fileName, const char* filePath)
	{
		//���łɃ��[�h�ς݂ł���΂��̃A�h���X��Ԃ�
		if (meshes.count((string)filePath + (string)fileName) == 1) {
			*ret_mesh = &meshes[(string)filePath + (string)fileName];
			return S_OK;
		}

		HRESULT hr = S_OK;
		vector<Mesh::mesh> skinMeshes;

#pragma region Load FBX

		// FbxManager��FbxScene��FbxImporter�I�u�W�F�N�g���쐬
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

		// �m�[�h�����̎擾�@(���݂̓��b�V���̂�)
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
		vector<Mesh::VertexFormat> vertices;
		vector<u_int> indices;
		u_int vertex_count = 0;

		skinMeshes.resize(fetched_meshes.size());
		for (size_t i = 0; i < fetched_meshes.size(); i++)
		{
			FbxMesh* fbxMesh = fetched_meshes.at(i)->GetMesh();
			Mesh::mesh& mesh = skinMeshes.at(i);

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

			// �}�e���A���̎擾
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
								// ��������
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

							// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� 
						//	ResourceManager::CreatePsFromCso("./DefaultShader/default_ps.cso", mesh.subsets.at(index_of_material).pixelShader.GetAddressOf());
						}
					}
					else
					{
						// �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� 
					//	ResourceManager::CreatePsFromCso("./DefaultShader/default_ps.cso", mesh.subsets.at(index_of_material).pixelShader.GetAddressOf());
					}
				}
			}

			// �A�j���[�V�����̎擾
			fetch_animations(fbxMesh, mesh.skeletalAnimation);

			// �{�[���̃E�G�C�g�̎擾
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
					// ���_
					const int index_of_control_point = fbxMesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
					vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
					vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
					vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
					// �@��
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


#pragma endregion

			// ���_���E�C���f�b�N�X���̃Z�b�g
#pragma region Set VertexData and IndexData

		// ���_�o�b�t�@�̐ݒ�
			D3D11_BUFFER_DESC vertexDesc = {};
			vertexDesc.ByteWidth = sizeof(Mesh::VertexFormat) * vertices.size();   // �o�b�t�@�̃T�C�Y	
			vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;	        // �o�b�t�@�̓ǂݏ����@
			vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // �p�C�v���C���ɂǂ��o�C���h���邩�w��
			vertexDesc.CPUAccessFlags = 0;                      // CPU�̃A�N�Z�X�t���O�@0�ŃA�N�Z�X���Ȃ�
			vertexDesc.MiscFlags = 0;                           // ���̑��̃t���O
			vertexDesc.StructureByteStride = 0;                 // �o�b�t�@�\���̂̏ꍇ�̗v�f��

			// ���_�o�b�t�@�ɒ��_�f�[�^�����邽�߂̐ݒ�
			D3D11_SUBRESOURCE_DATA vertexSubResource = {};
			vertexSubResource.pSysMem = &vertices[0];   // �������f�[�^�̃|�C���^
			vertexSubResource.SysMemPitch = 0;          // ���_�o�b�t�@�ł͎g��Ȃ�
			vertexSubResource.SysMemSlicePitch = 0;     // ���_�o�b�t�@�ł͎g��Ȃ�

			// ���_�o�b�t�@�̐���
			hr = Systems::Device->CreateBuffer(&vertexDesc, &vertexSubResource, mesh.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#pragma endregion

			// �C���f�b�N�X�o�b�t�@�I�u�W�F�N�g�̐���
#pragma region Create IndexBuffer

			// index
			//CreateIndices(fbxMesh, &indices);

			D3D11_BUFFER_DESC indexDesc = {};
			indexDesc.ByteWidth = indices.size() * sizeof(u_int);          // �o�b�t�@�̃T�C�Y	
			indexDesc.Usage = D3D11_USAGE_IMMUTABLE;	          // �o�b�t�@�̓ǂݏ����@
			indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;    // �p�C�v���C���ɂǂ��o�C���h���邩�w��
			indexDesc.CPUAccessFlags = 0;                    // CPU�̃A�N�Z�X�t���O�@0�ŃA�N�Z�X���Ȃ�
			indexDesc.MiscFlags = 0;                           // ���̑��̃t���O
			indexDesc.StructureByteStride = 0;                 // �o�b�t�@�\���̂̏ꍇ�̗v�f��

			D3D11_SUBRESOURCE_DATA indexSubResource = {};
			indexSubResource.pSysMem = &indices[0];   // �������f�[�^�̃|�C���^
			indexSubResource.SysMemPitch = 0;        // ���_�o�b�t�@�ł͎g��Ȃ�
			indexSubResource.SysMemSlicePitch = 0;   // ���_�o�b�t�@�ł͎g��Ȃ�

			hr = Systems::Device->CreateBuffer(&indexDesc, &indexSubResource, mesh.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

#pragma endregion

		// FBX���[�h����
		meshes[(string)filePath + (string)fileName] = skinMeshes;
		*ret_mesh = &meshes[(string)filePath + (string)fileName];

		manager->Destroy();

		return hr;
	}


	// �A�j���[�V�������擾����
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


	// �{�[���s����擾����
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

	// �{�[���̃E�G�C�g���擾����
	// FBXMesh*                                    �ǂݍ���FBXMesh
	// vector<bone_influences_per_control_point>&  �擾�����E�G�C�g�̕ۑ���
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


}