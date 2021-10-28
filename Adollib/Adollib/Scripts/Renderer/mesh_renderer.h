#pragma once

#include <vector>
#include <string>
#include <DirectXCollision.h>
#include "renderer_base.h"
#include "mesh.h"

namespace Adollib {
	class Gameobject;
	class Material;
	class Texture;

	class Mesh_renderer : public Renderer_base {

	public:

	public:
		void load_texture(const wchar_t* filename = nullptr);

		void calculate_boundingbox();

	public:

		void init() override {};
		void render(const Frustum_data& frustum_data) override;


	};
}