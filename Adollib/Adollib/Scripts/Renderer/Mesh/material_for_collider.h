#pragma once

#include <map>
#include <memory>
#include "mesh.h"
#include "../Shader/shader.h"
#include "../../Physics/ALP_Collider.h"
#include "../../Physics/collider_shape.h"

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

				float shininess;  // �X�y�L�����W��

				//float intensity;
			};
			//���_�t�H�[�}�b�g
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

			static ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@
			static ComPtr<ID3D11Buffer> Mat_cb; //material�p�o�b�t�@

			static std::map<ALPCollider_shape_type, std::vector<Mesh::mesh>*> meshes; //mesh
			static Shader shader; //shader
		public:


			static void initialize();
			static void render_collider(const Physics_function::ALP_Collider*);
			static void render_joint(const Physics_function::ALP_Joint*);

			//���ۂ�14-DOP�����`�悪�߂�ǂ������̂�AABB�ŕ`��
			static void render_AABB(const Physics_function::ALP_Collider*);

			static void render_box(const Collider_shape* R,const Vector3& color);
			static void render_sphere(const Collider_shape* R, const Vector3& color);
			static void render_meshcoll(const Collider_shape* R, const Vector3& color);
			static void render_capsule(const Collider_shape* R, const Vector3& color);

		};

	}


}