#pragma once

#include <map>
#include <memory>
#include "mesh.h"
#include "shader.h"
#include "rigit_body.h"

namespace Adollib {

	class Collider_renderer {

		//object
		struct ConstantBufferPerGO {
			DirectX::XMFLOAT4X4 world;
		};
		//material
		struct ConstantBufferPerMaterial
		{
			XMFLOAT4X4 boneTransforms[MAX_BONES];

			XMFLOAT4 materialColor;
			XMFLOAT4 ambientColor;

			float shininess;  // �X�y�L�����W��

			//float intensity;
		};
		//���_�t�H�[�}�b�g
		struct VertexFormat
		{
#define MAX_BONE_INFLUENCES (4)
			XMFLOAT3 position;
			XMFLOAT3 normal;
			XMFLOAT2 texcoord;
			FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 0 };
			INT bone_indices[MAX_BONE_INFLUENCES] = {};
		};
		static Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout; 

		static ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@
		static ComPtr<ID3D11Buffer> Mat_cb; //material�p�o�b�t�@

		static std::map<Rigitbody_shape, std::vector<Mesh::mesh>*> meshes; //mesh
		static Shader shader; //shader
	public:


		static void initialize();
		static void render(const Rigitbody*);

	};





}