#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include <wrl.h>
#include <fbxsdk.h>
#include "math.h"

#define MAX_BONES (4)

namespace Adollib
{

	class Mesh
	{
	private:
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;

	public:

		// 頂点フォーマット
		struct VertexFormat
		{
#define MAX_BONE_INFLUENCES (4)
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 texcoord;
			FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 0 };
			INT bone_indices[MAX_BONE_INFLUENCES] = {};
		};

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
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
			std::vector<subset> subsets;
			DOP::DOP_14 dop7; //meshのローカル座標系でのDOP_6
			DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			std::vector<skeletal_animation> skeletalAnimation;
		};

	public:
		Mesh() {};

		//Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = nullptr;
		//Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = nullptr;
		//vector<subset> subsets;
		//XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		//vector<skeletal_animation> skeletalAnimation;


	};
}