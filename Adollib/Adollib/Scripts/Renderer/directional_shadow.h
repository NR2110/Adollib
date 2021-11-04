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

	class Directional_shadow {

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> view_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> projection_cb;
		Microsoft::WRL::ComPtr<ID3D11Buffer> shadow_viewprojection_cb;

		std::shared_ptr<Texture> shadow_texture = nullptr;
		std::shared_ptr<Shader> shader = nullptr;

	public:
		float fov = 120.0f;
		float aspect = 1;
		float nearZ = 0.1f;
		float farZ = 1000.0f;

		Vector3 position;
		Vector3 direction;
		Matrix44 viewprojection;

	public:

		void awake();

		void setup();

		void shader_activate();

		// shadow_‚Ì[“xî•ñ‚ğshader‚É
		void set_ShaderResourceView();

		// frustumî•ñ‚ÌŒvZ
		Frustum_data calculate_frustum_data();
	};

};