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
		Microsoft::WRL::ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D11Buffer> Mat_cb; //�}�e���A�����p�o�b�t�@

		std::vector<Mesh::mesh>* meshes = nullptr; //mesh��� resorcemanager�ɕۑ����ꂽ���̂ւ̃|�C���^

		std::shared_ptr<Material> material = nullptr; //material���

	private:
		std::list<Renderer_base*>::iterator this_itr; //renderer_manager�ɕۑ����ꂽ���g��itr

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

		// shader��activate
		void shader_activete();

		// mesh��unique�Ȑ���(�A�h���X��cast�����l)
		long get_meshnum() const { return reinterpret_cast<long>(meshes); };
		// material��unique�Ȑ���(�A�h���X��cast�����l)
		long get_materialnum() const { return reinterpret_cast<long>(material.get()); };

		// �n���ꂽinstance�Ƀf�[�^������
		void set_instance(Instance& instance);

		// frustum�Ɋ܂܂�Ă��邩���m�F����
		virtual bool check_frustum(const Frustum_data& data) = 0;

	protected:
		virtual void init() = 0;  //awake�̒��ŌĂ� �h���N���X�p��initialize

	public:
	    void awake() override final;

		virtual void render(const Frustum_data& frustum_data) = 0;

		virtual void render_instancing(Microsoft::WRL::ComPtr<ID3D11Buffer>& instance_buffer, int bufferStart, int bufferCount) = 0;

		// Hierarchy�̕\��(Imgui�̊֐� Imgui::begin,Imgui::end�͂���Ȃ�)
		virtual void Update_hierarchy() {};

		void finalize() override final;

	};
}