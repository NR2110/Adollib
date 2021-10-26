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

	public:

	public:
		void load_texture(const wchar_t* filename = nullptr);

	public:

		void render();


	};
}