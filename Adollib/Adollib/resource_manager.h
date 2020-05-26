#pragma once
#include <d3d11.h>
#include "mesh.h"
#include "texture.h"
#include "shader.h"

namespace Adollib
{
	class ResourceManager
	{
	public:
		static HRESULT CreateVsFromCso(const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
		static HRESULT CreatePsFromCso(const char* cso_name, ID3D11PixelShader** pixel_shader);
		static HRESULT CreateGsFromCso(const char* cso_name, ID3D11GeometryShader** geometry_shader);
		static HRESULT CreateHsFromCso(const char* cso_name, ID3D11HullShader** hull_shader);
		static HRESULT CreateDsFromCso(const char* cso_name, ID3D11DomainShader** domain_shader);
		static HRESULT LoadTextureFromFile(const wchar_t* file_name, ID3D11ShaderResourceView** pSRV, D3D11_TEXTURE2D_DESC* pTex2dDesc);
		static HRESULT CreateModelFromFBX(vector<Mesh::mesh>& meshes, const char* fileName, const char* filePath = "");

	private:
		ResourceManager() {};
		~ResourceManager() {};
		static void fetch_bone_influences(const FbxMesh* fbx_mesh, vector<Mesh::bone_influences_per_control_point>& influences);
		static void fetch_bone_matrices(FbxMesh* fbx_mesh, vector<Mesh::bone>& skeletal, FbxTime time);
		static void fetch_animations(FbxMesh* fbx_mesh, vector<Mesh::skeletal_animation>& skeletal_animation, u_int sampling_rate = 0);

		static std::map<std::string, std::vector<Mesh::mesh>> meshes;
		static std::map<std::wstring, Texture> texturs;
		struct VS_resorce {
			ID3D11VertexShader* VSshader;
			ID3D11InputLayout* layout;

		};
		static std::map<std::string, VS_resorce*>   VSshaders;
		static std::map<std::string, ID3D11PixelShader*>    PSshaders;
		static std::map<std::string, ID3D11GeometryShader*> GSshaders;
		static std::map<std::string, ID3D11HullShader*>     HSshaders;
		static std::map<std::string, ID3D11DomainShader*>   DSshaders;
	};
}