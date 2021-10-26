#pragma once

#include <vector>
#include <string>
#include "../Object/component.h"
#include "mesh.h"

namespace Adollib {
	class Gameobject;
	class Material;
	class Texture;

	class Renderer_base : public Component {
	protected:
		ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@
		ComPtr<ID3D11Buffer> Mat_cb; //�}�e���A�����p�o�b�t�@

		std::vector<Mesh::mesh>* meshes = nullptr; //mesh��� resorcemanager�ɕۑ����ꂽ���̂ւ̃|�C���^

		std::shared_ptr<Material> material = nullptr; //material���

		std::shared_ptr<Texture> texture = nullptr; //texture���

	private:
		std::list<Renderer_base*>::iterator this_itr; //renderer_manager�ɕۑ����ꂽ���g��itr


	public:
		int animeIndex = 0;

	public:
		void set_material(std::shared_ptr<Material>& l_material) { material = l_material; };
		void set_meshes(std::vector<Mesh::mesh>* l_meshes) { meshes = l_meshes; };
		void set_texture(std::shared_ptr<Texture>& l_texture) { texture = l_texture; };

		std::shared_ptr<Material> get_material() { return material; };
		std::shared_ptr<Texture>  get_texture() { return texture; };

	public:
	    void awake() override final;

		virtual void render() = 0;

		void finalize() override final;

	};
}