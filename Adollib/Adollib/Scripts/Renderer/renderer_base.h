#pragma once

#include <vector>
#include <string>
#include "../Object/component.h"
#include "mesh.h"
#include "frustum_data.h"
#include "instance.h"

namespace Adollib {
	class Gameobject;
	class Material;
	class Texture;

	class Renderer_base : public Component {
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> Mat_cb; //マテリアル情報用バッファ

		std::vector<Mesh::mesh>* meshes = nullptr; //mesh情報 resorcemanagerに保存された実体へのポインタ

		std::shared_ptr<Material> material = nullptr; //material情報

	private:
		std::list<Renderer_base*>::iterator this_itr; //renderer_managerに保存された自身のitr

	public:
		bool is_use_instancing = true;

		int animeIndex = 0;

		Vector4 color = Vector4(1, 1, 1, 1);

		bool is_render = true;

	public:
		virtual void set_material(std::shared_ptr<Material> l_material) { material = l_material; };
		virtual void set_meshes(std::vector<Mesh::mesh>* l_meshes) { meshes = l_meshes; };

		std::shared_ptr<Material> get_material() { return material; };
		std::vector<Mesh::mesh>*  get_mesh() { return meshes; };

		// shaderのactivate
		void shader_activete();

		// meshのuniqueな数字(アドレスをcastした値)
		long get_meshnum() const { return reinterpret_cast<long>(meshes); };
		// materialのuniqueな数字(アドレスをcastした値)
		long get_materialnum() const { return reinterpret_cast<long>(material.get()); };

		// 渡されたinstanceにデータを入れる
		void set_instance(Instance& instance);

		// frustumに含まれているかを確認する
		virtual bool check_frustum(const Frustum_data& data) = 0;

	protected:
		virtual void init() = 0;  //awakeの中で呼ぶ 派生クラス用のinitialize

	public:
	    void awake() override final;

		virtual void render(const Frustum_data& frustum_data) = 0;

		virtual void render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) = 0;

		// Hierarchyの表示(Imguiの関数 Imgui::begin,Imgui::endはいらない)
		virtual void Update_hierarchy() {};

		void finalize() override final;

	};
}