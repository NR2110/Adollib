#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

#include "Shader/shader.h"


namespace Adollib {
	class Texture;

	class Sprite_data {
	public:
		float dx = 0;
		float dy = 0;
		float dw = 0;
		float dh = 0;
		float sx = 0;
		float sy = 0;
		float sw = 0;
		float sh = 0;

		float angle = 0.0f;
		float alpha = 1.0f;
	};

	class Sprite {

	private:
		ComPtr<ID3D11Buffer> VertexBuffer;
		ComPtr<ID3D11DepthStencilState> DepthStencilState;
		std::shared_ptr<Texture> texture = nullptr;

	public:
		Sprite_data sprite_data;

		Shader shader;

	public:

		Sprite();

		void set_texture(std::shared_ptr<Texture>& texture);

		void set_default();

		void render();

		//void Set_texture(u_int slot) { texture.Set(slot); }

	};



}