#pragma once
#include <d3d11.h>
#include <unordered_map>
#include "../Renderer/mesh.h"
#include "../Renderer/texture.h"
#include "../Renderer/Shader/shader.h"

namespace Adollib
{
	//next ResourceManagerÇÃreleaceèàóù
	class ResourceManager
	{
	public:
		static HRESULT CreateVsFromCso(const char* cso_name, Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertex_shader, Microsoft::WRL::ComPtr<ID3D11InputLayout>& input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);
		static HRESULT CreatePsFromCso(const char* cso_name, Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixel_shader);
		static HRESULT CreateGsFromCso(const char* cso_name, Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometry_shader);
		static HRESULT CreateHsFromCso(const char* cso_name, Microsoft::WRL::ComPtr<ID3D11HullShader>& hull_shader);
		static HRESULT CreateDsFromCso(const char* cso_name, Microsoft::WRL::ComPtr<ID3D11DomainShader>& domain_shader);
		static HRESULT CreateCsFromCso(const char* cso_name, Microsoft::WRL::ComPtr<ID3D11ComputeShader>& computeShader);
		static HRESULT LoadTextureFromFile(const wchar_t* file_name, ID3D11ShaderResourceView** pSRV, D3D11_TEXTURE2D_DESC* pTex2dDesc);
		static HRESULT CreateModelFromFBX(std::vector<Mesh::mesh>** meshes, const char* fileName, const char* filePath = "");

		static void destroy();
	private:
		ResourceManager() {};
		~ResourceManager() {};
		static void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<Mesh::bone_influences_per_control_point>& influences);
		static void fetch_bone_matrices(FbxMesh* fbx_mesh, std::vector<Mesh::bone>& skeletal, FbxTime time);
		static void fetch_animations(FbxMesh* fbx_mesh, std::vector<Mesh::skeletal_animation>& skeletal_animation, u_int sampling_rate = 0);

		static std::unordered_map<std::string, std::vector<Mesh::mesh>> meshes;
		static std::unordered_map<std::wstring, Texture> texturs;
		struct VS_resorce {
			Microsoft::WRL::ComPtr<ID3D11VertexShader> VSshader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
		};

		static std::unordered_map <std::string, VS_resorce>			   VSshaders;
		static std::unordered_map <std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>>    PSshaders;
		static std::unordered_map <std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> GSshaders;
		static std::unordered_map <std::string, Microsoft::WRL::ComPtr<ID3D11HullShader>>     HSshaders;
		static std::unordered_map <std::string, Microsoft::WRL::ComPtr<ID3D11DomainShader>>   DSshaders;
		static std::unordered_map <std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>>  CSshaders;
	};
}