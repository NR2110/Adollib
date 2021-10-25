#pragma once

#include <vector>
#include <string>
#include "../Object/component.h"
#include "mesh.h"
#include "material.h"

namespace Adollib {
	class Gameobject;

	class Renderer : public Component {

	private:
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

		std::vector<Mesh::mesh>* meshes = nullptr; //

		std::shared_ptr<Material> material = nullptr; //

		std::list<Renderer*>::iterator this_itr; //renderer_managerに保存された自身のitr

		ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
		ComPtr<ID3D11Buffer> Mat_cb; //マテリアル情報用バッファ

	public:
		int animeIndex = 0;

	public:
		void set_material(std::shared_ptr<Material>& l_material) { material = l_material; };
		void set_meshes(std::vector<Mesh::mesh>* l_meshes) { meshes = l_meshes; };

	public:


		void awake() override;

		void render();

		void finalize() override;

	};
}