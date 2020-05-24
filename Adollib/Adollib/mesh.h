#pragma once

#include <fbxsdk.h>

#include "vector.h"
#include "matrix.h"
#include "texture.h"
#include "shader.h"
#include "object.h"

#define MAX_BONES (160)

namespace Adollib {
	class Mesh : object {
	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer; // 定数バッファ　シェーダーに値を与えるためのバッファ
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateLine; // ライン描画
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStateFill; // ソリッド描画
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

		matrix world;
		matrix worldViewProjection;

		float timer = 0;

	public:

		vector3 scale = { 1, 1, 1 };
		vector3 rot = { 0, 180, 0 };
		vector3 pos = {};
		int animeIndex = 0;
		int isLineDraw = FALSE;

		// 頂点フォーマット
		struct VertexFormat
		{
#define MAX_BONE_INFLUENCES (4)
			vector3 position;
			vector3 normal;
			DirectX::XMFLOAT2 texcoord;
			FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 0 };
			INT bone_indices[MAX_BONE_INFLUENCES] = {};
		};

		// 定数バッファ
		struct ConBuffer
		{
			matrix world;
			matrix worldViewProjection;

			matrix boneTransforms[MAX_BONES];

			vector4 materialColor;
			vector4 lightDirection;
			vector4 lightColor;

			vector4 ambientColor;

			vector4 emmisionColor;

			vector4 specular;   // specularカラー
			vector4 cameraWPos; // ワールド座標のカメラの位置
			float shininess;  // スペキュラ係数

			float intensity;

			float time;
		};

		// マテリアル
		struct material
		{
			vector4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceVirw = nullptr;
		};

		// サブセット
		struct subset
		{
			u_int indexStart = 0; // start number of index buffer
			u_int indexCount = 0; // number of vertices (indices)
			//material diffuse;
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
			matrix transform;
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
			matrix globalTransform = matrix_identity();
			std::vector<skeletal_animation> skeletalAnimation;
		};
		std::vector<mesh> meshes;

		// convert coordinate system from 'UP:+Z FRONT:+Y RIGHT-HAND' to 'UP:+Y FRONT:+Z LEFT-HAND'
		matrix coordinateConversion = matrix_identity();

	public:
		Mesh(ID3D11Device* device, const char* fbx_filename, const char* filePath);
		~Mesh() {};
		// 描画
		void Render(
			ID3D11DeviceContext*, //デバイスコンテキスト
			const vector4&, //ライト進行方向
			float elapsedTime
		);

	private:
		void CreateIndices(FbxMesh*, std::vector<u_int>*);


	};


}