#pragma once

#include "systems.h"
#include "mesh.h"

namespace Adollib {
	class Mesh_manager {
	public:
		static HRESULT CreateVsFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
		static HRESULT CreatePsFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);
		static HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);
		static HRESULT CreateModelFromFBX(ID3D11Device* device, std::vector<Mesh>& meshes, const char* fileName, const char* filePath);

	private:
		Mesh_manager() {};
		~Mesh_manager() {};
		static void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<Mesh::bone_influences_per_control_point>& influences);
		static void fetch_bone_matrices(FbxMesh* fbx_mesh, std::vector<Mesh::bone>& skeletal, FbxTime time);
		static void fetch_animations(FbxMesh* fbx_mesh, std::vector<Mesh::skeletal_animation>& skeletal_animation,
			u_int sampling_rate = 0);
	};


	};


}