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
		Vector3 position; //cameraの座標
		Vector3 rotate; //cameraの回転

		float fov = 120.0f;
		float aspect = 1;
		float nearZ = 0.1f;
		float farZ = 1000.0f;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

		std::vector<Mesh::mesh>* meshes = nullptr; //mesh情報 resorcemanagerに保存された実体へのポインタ
		std::shared_ptr<Texture> sky_texture = nullptr;
		std::shared_ptr<Shader>  shader = nullptr;

		Microsoft::WRL::ComPtr<ID3D11Buffer> world_cb; //WVP行列用バッファ
		Microsoft::WRL::ComPtr<ID3D11Buffer> Mat_cb; //マテリアル情報用バッファ


	public:

		void awake();

		void render();

		void shader_activate();

	};
}