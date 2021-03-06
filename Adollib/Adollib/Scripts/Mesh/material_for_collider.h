#pragma once

#include <map>
#include <memory>
#include "mesh.h"
#include "../Shader/shader.h"
#include "../Physics/ALP_Collider.h"
#include "../Physics/collider_shape.h"

namespace Adollib {
	namespace Physics_function {

		class Collider_renderer {

			//object
			struct ConstantBufferPerGO {
				DirectX::XMFLOAT4X4 world;
			};
			//material
			struct ConstantBufferPerMaterial
			{
				DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES];

				DirectX::XMFLOAT4 materialColor;
				DirectX::XMFLOAT4 ambientColor;

				float shininess;  // スペキュラ係数

				//float intensity;
			};
			//頂点フォーマット
			struct VertexFormat
			{
#define MAX_BONE_INFLUENCES (4)
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT3 normal;
				DirectX::XMFLOAT2 texcoord;
				FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 0 };
				INT bone_indices[MAX_BONE_INFLUENCES] = {};
			};
			static Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;

			static ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
			static ComPtr<ID3D11Buffer> Mat_cb; //material用バッファ

			static std::map<ALPCollider_shape_type, std::vector<Mesh::mesh>*> meshes; //mesh
			static Shader shader; //shader
		public:


			static void initialize();
			static void render_collider(const Physics_function::ALP_Collider*);

			//実際は14-DOPだが描画がめんどくさいのでAABBで描画
			static void render_AABB(const Physics_function::ALP_Collider*);

			static void render_box(const Collider_shape* R,const Vector4 color = Vector4(-1, -1, -1, -1));
			static void render_sphere(const Collider_shape* R,const Vector4 color = Vector4(-1, -1, -1, -1));
			static void render_meshcoll(const Collider_shape* R,const Vector4 color = Vector4(-1, -1, -1, -1));
			static void render_capsule(const Collider_shape* R,const Vector4 color = Vector4(-1, -1, -1, -1));

		};

	}


}