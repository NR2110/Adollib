#pragma once

#include <vector>
#include <string>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {
	class Gameobject;
	class Material;
	class Texture;

	class Sprite_renderer : public Renderer_base {
	private:
		ComPtr<ID3D11Buffer> VertexBuffer;

	public:
		Vector2 uv_pos[4] = {
			Vector2(0, 0),
			Vector2(0, 1),
			Vector2(1, 0),
			Vector2(1, 1)
		};


	public:
		void load_texture(const wchar_t* filename = nullptr);

	public:

		void init() override;
		void render() override;


	};
}