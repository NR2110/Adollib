#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <wrl.h>
#include <fbxsdk.h>
#include "../Math/math.h"

#include <d3dcompiler.h>
#include <memory>
#include <wrl.h>
#include <assert.h>

#include "../Renderer/Shader/vertex_format.h"

namespace Adollib
{

	class Mesh
	{
	private:
		//Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;

		//ComPtr<ID3D11Buffer> mesh_cb; //mesh_world行列用バッファ

	public:

		// マテリアル
		struct material
		{
			DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceVirw = nullptr;
		};

		// サブセット
		struct subset
		{
			u_int indexStart = 0; // start number of index buffer
			u_int indexCount = 0; // number of vertices (indices)
			material diffuse;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
		};

		// ボーンウエイト
		struct bone_influence
		{
			int index = 0; // index of bone
			float weight = 0; // weight of bone
		};
		typedef std::vector<bone_influence> bone_influences_per_control_point;

		// ボーン
		struct bone
		{
			DirectX::XMFLOAT4X4 transform;
		};

		// アニメーションデータ
		typedef std::vector<bone> skeletal;
		struct skeletal_animation : public std::vector<skeletal>
		{
			float sampling_time = 1 / 24.0f;
			float animation_tick = 0.0f;
		};

		// メッシュ
		struct mesh
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> mesh_cb; //mesh_world行列用バッファ
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
			std::vector<VertexFormat> vertices;
			std::vector<u_int> indexces;
			std::vector<subset> subsets;
			DOP::DOP_14 dop; //meshのローカル座標系でのk-DOP
			DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			std::vector<skeletal_animation> skeletalAnimation;
		};

	public:
		Mesh() {};
		//void initialize();
		//void set_constant_buffer(const DirectX::XMFLOAT4X4& globalTransform);

		//Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		//Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
		//vector<subset> subsets;
		//XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		//vector<skeletal_animation> skeletalAnimation;


	};
}