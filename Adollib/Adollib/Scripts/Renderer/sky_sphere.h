#pragma once
#include "../Math/math.h"
#include "frustum_data.h"
#include <wrl.h>
#include <memory>
#include <d3d11.h>

namespace Adollib {
	class Gameobject;
	class Material;
	class Texture;
	class Shader;

	class Sky_sphere {
	public:
		Vector3 position; //camera‚ÌÀ•W
		Vector3 rotate; //camera‚Ì‰ñ“]

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;

		std::shared_ptr<Texture> sky_texture = nullptr;
		std::shared_ptr<Shader>  shader = nullptr;

	public:

		void awake();

		void setup();

		void shader_activate();

	};
}