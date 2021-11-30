#pragma once
#include "../Math/math.h"
#include "frustum_data.h"
#include "mesh.h"
#include <wrl.h>
#include <memory>
#include <d3d11.h>

namespace Adollib {
	class Gameobject;
	class Texture;
	class Shader;

	class Sky_sphere {
	public:
		Vector3 position; //camera�̍��W
		Vector3 rotate; //camera�̉�]

		float fov = 120.0f;
		float aspect = 1;
		float nearZ = 0.1f;
		float farZ = 1000.0f;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

		std::vector<Mesh::mesh>* meshes = nullptr; //mesh��� resorcemanager�ɕۑ����ꂽ���̂ւ̃|�C���^
		std::shared_ptr<Texture> sky_texture = nullptr;
		std::shared_ptr<Shader>  shader = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> world_cb; //WVP�s��p�o�b�t�@
		Microsoft::WRL::ComPtr<ID3D11Buffer> Mat_cb; //�}�e���A�����p�o�b�t�@


	public:

		void awake();

		void render();

		void shader_activate();

	};
}